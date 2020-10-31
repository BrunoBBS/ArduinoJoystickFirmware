#include <EEPROM.h>
#include <Joystick.h>
#include <math.h>
#include <stdlib.h>

#include "Bezier.hpp"

#define CALIBRATION_BUTTON 7
#define LED 13

#define CENTER 512

#define AXIS_ARRAY_SIZE 3

#define LINEAR 2
#define CURVE 1

// some pins can function as either digital or analog inputs, so we predefine
// which will operate in which mode
//                                 X   Y   Th
static const int axes_pins[] = { A0, A1, A6 };
static const int button_pins[] = { 3, 5, 6, 7, 8, 9, 14 };

struct {
    bool is_calibrated = false;
    uint16_t max[AXIS_ARRAY_SIZE];
    uint16_t min[AXIS_ARRAY_SIZE];
    Bezier curves[AXIS_ARRAY_SIZE];
} calibration_data;

void blink(int led, int cycles, int on_millis = 500, int delay_millis = -1)
{
    if (delay_millis < 0)
        delay_millis = on_millis;

    for (; cycles > 0; cycles--) {
        digitalWrite(led, HIGH);
        delay(on_millis);
        digitalWrite(led, LOW);
        delay(delay_millis);
    }
}

void calibration()
{
    // Properties
    int calibration_address = 0x00;

    // Fetch data
    bool force_calibrate = !digitalRead(CALIBRATION_BUTTON);
    if (force_calibrate)
        Serial.println("Force calibration");
    EEPROM.get(calibration_address, calibration_data);

    // Skip if calibrated
    if (calibration_data.is_calibrated && !force_calibrate) {
        Serial.println("Is already calibrated");
        return;
    }

    // Reset calibration data
    calibration_data.is_calibrated = false;
    for (int i = 0; i < AXIS_ARRAY_SIZE; i++) {
        calibration_data.min[i] = 0xFFFF;
        calibration_data.max[i] = 0x0000;
    }

    // Wait for button release
    Serial.println("Release calibration button to proceed...");
    while (!digitalRead(CALIBRATION_BUTTON)) {
    }

    // Calibrate
    digitalWrite(LED, HIGH);
    Serial.println("Move axes now");
    char count = 0;
    while (!calibration_data.is_calibrated) {
        delay(1);
        if (count++ > 6) {
            calibration_data.is_calibrated = !digitalRead(CALIBRATION_BUTTON);
            count = 0;
        }

        delay(50);
        Serial.println("========= Calibrating =========");
        for (int i = 0; i < AXIS_ARRAY_SIZE; i++) {

            uint16_t curr = analogRead(axes_pins[i]);
            Serial.print("Axis: ");
            Serial.print(i);
            Serial.print(" - Raw Value: ");
            Serial.println(curr);

            calibration_data.min[i] = min(curr, calibration_data.min[i]);
            calibration_data.max[i] = max(curr, calibration_data.max[i]);
        }
        Serial.println("===============================");
        Serial.println("Hold calibration button to finish...");
    }

    Serial.println("Done calibrating");
    digitalWrite(LED, LOW);

    calibration_data.curves[0] = Bezier(Point(0, 0), Point(0, 900), Point(1023, 100), Point(1023, 1023));
    calibration_data.curves[1] = Bezier(Point(0, 0), Point(0, 900), Point(1023, 100), Point(1023, 1023));

    // Save calibration
    EEPROM.put(calibration_address, calibration_data);
}

void setupPins(void)
{
    // first set all pins as HIGH to zero the pressed state
    for (int i = 2; i < 20; i++) {
        digitalWrite(i, HIGH);
    }

    // Set all the digital pins as inputs
    // with the pull-up enabled, except for the
    // two serial line pins
    for (int pin : button_pins) {
        pinMode(pin, INPUT_PULLUP);
    }

    for (int Ai : axes_pins) {
        pinMode(Ai, INPUT);
        digitalWrite(Ai, LOW);
    }

    pinMode(LED, OUTPUT);
}

int compare(const void* a, const void* b)
{
    int16_t x = *((int16_t*)a);
    int16_t y = *((int16_t*)b);
    return x - y;
}

// Median filter, use with odd values (non even).
int analogReadFilter(int pin, int total_reads)
{
    uint16_t* measurements = (uint16_t*)malloc(total_reads * sizeof(uint16_t));

    for (int i = 0; i < total_reads; i++)
        measurements[i] = analogRead(pin);

    qsort(measurements, total_reads, sizeof(uint16_t), compare);

    int filtered_val = measurements[total_reads / 2];

    free(measurements);
    return filtered_val;
}

int readToCurve(int reading, int index)
{
    return calibration_data.curves[index].eval(reading).y;
}

int hat(int hat_x, int hat_y)
{
    // evaluates if hat is in center
    if (hat_x > (1023 * 0.45) && hat_x < (1023 * 0.65) && hat_y > (1023 * 0.45) && hat_y < (1023 * 0.65)) {
        return JOYSTICK_HATSWITCH_RELEASE;

    } else {
        float deltaX = hat_x - CENTER;
        float deltaY = hat_y - CENTER;
        float rad = atan2(deltaY, deltaX);
        int deg = degrees(rad);
        if (deg < 0)
            deg += 360;

        // Make the hat only 4 way
        // up
        if (deg > 315 || deg < 45)
            deg = 0;
        // right
        else if (deg > 45 && deg < 135)
            deg = 90;
        // down
        else if (deg > 135 && deg < 225)
            deg = 180;
        // left
        else if (deg > 225 && deg < 315)
            deg = 270;

        return deg;
    }
}

int axisRead(int index, int filter_total, int option = CURVE)
{
    if (option == CURVE) {
        return readToCurve(
            map(
                analogReadFilter(
                    axes_pins[index],
                    filter_total
                ),
                calibration_data.min[index],
                calibration_data.max[index],
                0,
                1023
            ),
            index
        );
    }
    return map(
        analogReadFilter(
            axes_pins[index],
            filter_total
        ),
        calibration_data.min[index],
        calibration_data.max[index],
        0,
        1023
    );
}

int buttonRead(int index)
{
    return !digitalRead(button_pins[index]);
}

// ===========================================================================

Joystick_ joystick;

void setup()
{
    Serial.begin(9600);
    delay(2000);
    Serial.println("=================================");
    Serial.println("Joystick Initializing - baud 9600");
    Serial.println("=================================");
    Serial.println();
    Serial.println("Hold calibration button to force calibration...");
    for (int i = 0; i < 50; i++) {
        Serial.print("#");
        delay(50);
    }
    Serial.println();
    setupPins();
    calibration();
    joystick.begin();
}

void loop()
{
    // read the buttons
    for (int btn = 0; btn < (sizeof(button_pins) / sizeof(int)); btn++)
        joystick.setButton(btn, buttonRead(btn));

    // read the axes
    joystick.setYAxis(axisRead(0, 3, CURVE));
    joystick.setXAxis(axisRead(1, 3, CURVE));
    joystick.setZAxis(axisRead(2, 5, LINEAR));

    /* joystick.setRxAxis(constrain(map(analog(A8), */
    /* calibration_data.min[2], */
    /* calibration_data.max[2], */
    /* 0, */
    /* 1023), */
    /* 0, */
    /* 1023)); */
    /* joystick.setRyAxis(constrain(map(analog(A9), */
    /* calibration_data.min[3], */
    /* calibration_data.max[3], */
    /* 0, */
    /* 1023), */
    /* 0, */
    /* 1023)); */
    //Uncomment to enable HAT AS DIGITAL and COMMENT the Rx and Ry axes
    // read one joystick as a hatswitch
    // tranform x and y readings into an angle for the hatswitch
    // From here {
    /* int hat_x = analogRead(A8); */
    /* int hat_y = analogRead(A9); */

    /* joystick.setHatSwitch(0, hat(hat_x, hat_y)); */
    // To here }
}

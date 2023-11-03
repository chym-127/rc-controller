#include "esp32-hal-ledc.h"
#include "Arduino.h"
#include "Esc.h"

int getMin(int a, int b)
{
    if (a > b)
    {
        return b;
    }
    return a;
}

int getMax(int a, int b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

Esc::Esc()
{
    this->minUs = DEFAULT_uS_MIN;
    this->maxUs = DEFAULT_uS_MAX;
    this->pinNumber = -1;
    this->freq = DEFAULT_FREQ;
    this->timerWidth = TIMER_10_BIT;
}

// 配置通道
void Esc::setUp(uint8_t channel)
{
    this->setUp(channel, DEFAULT_FREQ, TIMER_10_BIT);
}

// 配置通道
void Esc::setUp(uint8_t channel, uint32_t freq, uint8_t resolution_bits)
{
    this->channel = channel;
    this->timerWidth = getMin(resolution_bits, TIMER_15_BIT);
    this->resolution = pow(2, this->timerWidth) - 1;
    this->resolutionMiddle = pow(2, this->timerWidth) / 2;
    this->freq = getMin(freq, MAX_FREQ);
    this->ticks = 1000 * 1000 / this->freq;
    ledcSetup(channel, freq, resolution_bits);
    Serial.print("FREQ: ");
    Serial.println(this->freq);
    Serial.print("RESOLUTION: ");
    Serial.println(this->resolution);
}

void Esc::changeFrequency(uint32_t freq)
{
    ledcChangeFrequency(this->channel, freq, this->timerWidth);
}

// 绑定GOIP
void Esc::attachPin(uint8_t pin)
{
    pinMode(pin, OUTPUT);
    ledcAttachPin(pin, this->channel);
}

// 写入脉宽
void Esc::write(uint32_t val, uint32_t min, uint32_t max, uint32_t oMin, uint32_t oMax)
{
    int duty = map(val, min, max, oMin, oMax);
    this->writeDuty(duty);
}

// 写入脉宽 微秒
void Esc::writeMicroseconds(uint32_t usec)
{
    int val = getMin(usec, this->ticks);
    int duty = usecToDuty(val);
    this->writeDuty(duty);
}

void Esc::writeDuty(uint32_t duty)
{
    Serial.print("DUTY");
    Serial.print("(total-");
    Serial.print((pow(2, this->timerWidth) - 1));
    Serial.print("): ");
    Serial.println(duty);
    ledcWrite(this->channel, duty);
}

void Esc::write(uint32_t duty)
{
    this->writeDuty(duty);
}

void Esc::write(uint32_t val, uint32_t min, uint32_t max)
{
    int duty = map(val, min, max, 0, this->resolution);
    ledcWrite(this->channel, duty);
}

int Esc::getTicks()
{
    return this->ticks;
}

uint32_t Esc::usecToDuty(uint32_t usec)
{
    return (pow(2, this->timerWidth) - 1) * (usec / this->ticks);
}

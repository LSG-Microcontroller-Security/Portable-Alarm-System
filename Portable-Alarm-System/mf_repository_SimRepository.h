// mf_repository_SimRepository.h
#pragma once

#include <stdint.h>
#include <mf_repository_ISerial.h>

#ifndef _ON_MOCKING_TESTS
#define _ON_MOCKING_TESTS 0U
#endif

class SimRepository {
public:
    explicit SimRepository(ISerial& serial);
    SimRepository(ISerial& serial, unsigned long baud_rate);
    SimRepository(ISerial& serial, uint8_t sleep_pin, unsigned long baud_rate, uint8_t boot_pin = 255U);

#if _ON_MOCKING_TESTS
    virtual ~SimRepository() = default;
    virtual void begin(unsigned long baud_rate);
    virtual void call(const char* number);
    virtual void hangUp();
    virtual void enableIncomingCall(uint8_t number_of_rings);
    virtual void disableIncomingCall();
    virtual void setSmsTextMode();
    virtual void setSmsPduMode();
    virtual void sendSms(const char* number, const char* message);
    virtual void initSmsReception();
    virtual bool setNetlightEnabled(bool enabled);
    virtual bool readSms(uint8_t index, char* message);
    virtual bool readSms(uint8_t index, char* sender, uint8_t sender_capacity, char* message, uint8_t message_capacity);
    virtual void deleteAllSms();
    virtual int getSmsCount();
    virtual bool deleteSmsAt(uint8_t index);
    virtual uint8_t findSmsWithContent(const char* pattern);
    virtual bool isRegisteredToNetwork();
    virtual int getSignalStrength();
    virtual bool isGprsAttached();
    virtual bool enterSleepMode();
    virtual bool exitSleepMode();
    virtual bool turn_on_module();
    virtual bool turn_off_module();
    virtual bool is_module_on();
    virtual bool listen();
    virtual bool is_listening();
    virtual bool stop_listening();
    virtual void delay(unsigned long ms);
#else
    ~SimRepository() = default;
    void begin(unsigned long baud_rate);
    void call(const char* number);
    void hangUp();
    void enableIncomingCall(uint8_t number_of_rings);
    void disableIncomingCall();
    void setSmsTextMode();
    void setSmsPduMode();
    void sendSms(const char* number, const char* message);
    void initSmsReception();
    bool setNetlightEnabled(bool enabled);
    bool readSms(uint8_t index, char* message);
    bool readSms(uint8_t index, char* sender, uint8_t sender_capacity, char* message, uint8_t message_capacity);
    void deleteAllSms();
    int getSmsCount();
    bool deleteSmsAt(uint8_t index);
    uint8_t findSmsWithContent(const char* pattern);
    bool isRegisteredToNetwork();
    int getSignalStrength();
    bool isGprsAttached();
    bool enterSleepMode();
    bool exitSleepMode();
    bool turn_on_module();
    bool turn_off_module();
    bool is_module_on();
    bool listen();
    bool is_listening();
    bool stop_listening();
    void delay(unsigned long ms);
#endif

private:
    void clear_receive_buffer();
    bool wait_for_pattern(const char* pattern, unsigned long timeout_ms);
    uint8_t read_response(char* buffer, uint8_t buffer_size, unsigned long timeout_ms);

private:
    ISerial& serial_;
    uint8_t sleep_pin_;
    uint8_t boot_pin_;
    bool is_sms_receive_initialized_;
    bool is_call_disabled_;
};

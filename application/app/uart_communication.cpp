#include "uart_communication.h"
#include "version.h"
#include "logger.h"

#include "communication_datatypes/firmware_info.h"
#include "communication_datatypes/status_data.h"
#include "communication_datatypes/channel_settings.h"
#include "communication_datatypes/monitoring_data.h"
#include "communication_datatypes/remote_control_data.h"
#include "communication_datatypes/error_status.h"

#define APP_VER "AppBS v" APP_VERSION

#pragma GCC push_options
#pragma GCC optimize ("Og")

UARTCommunication::UARTCommunication(State &state, IUart &uart)
    : mState(state), mUart(uart), mProtocol()
{
    // Register command handlers for protocol communications
    mProtocol.registerCmd('c', [this](const uint8_t&in, uint8_t &out, size_t &outlen) { return this->sendChannelSettings(in, out, outlen); });
    mProtocol.registerCmd('C', [this](const uint8_t &in, uint8_t &out, size_t &outlen) { return this->updateChannelSettings(in, out, outlen); });
    mProtocol.registerCmd('e', [this](const uint8_t &in, uint8_t &out, size_t &outlen) { return this->sendErrors(in, out, outlen); });
    mProtocol.registerCmd('v', [this](const uint8_t &in, uint8_t &out, size_t &outlen) { return this->sendFirmwareInfo(in, out, outlen); });
    mProtocol.registerCmd('r', [this](const uint8_t &in, uint8_t &out, size_t &outlen) { return this->resetDevice(in, out, outlen); });
    mProtocol.registerCmd('S', [this](const uint8_t &in, uint8_t &out, size_t &outlen) { return this->sendStatus(in, out, outlen); });
    mProtocol.registerCmd('m', [this](const uint8_t &in, uint8_t &out, size_t &outlen) { return this->sendMonitoringData(in, out, outlen); });
    mProtocol.registerCmd('l', [this](const uint8_t &in, uint8_t &out, size_t &outlen) { return this->remoteControl(in, out, outlen); });
}

bool UARTCommunication::sendFirmwareInfo(const uint8_t &in, uint8_t &out, size_t &outlen) {
    LOG << "Getting app version";
    FirmwareInfo &firmwareInfo = reinterpret_cast<FirmwareInfo&>(out);
    strcpy(firmwareInfo.app_version, APP_VER);
    strcpy(firmwareInfo.hardware_version, HARDWARE_VERSION);
    strcpy(firmwareInfo.build_date, __DATE__);
    strcpy(firmwareInfo.build_time, __TIME__);
    strcpy(firmwareInfo.git_commit, GIT_COMMIT);
    strcpy(firmwareInfo.app_version, APP_VER);
    
    outlen = sizeof(firmwareInfo);
    return true;
}

bool UARTCommunication::sendStatus(const uint8_t &in, uint8_t &out, size_t &outlen) {
    LOG << "Getting status";
    StatusData &statusData = reinterpret_cast<StatusData&>(out);
    statusData.ldg_gear_switch = mState.mSwitches.mLdgGearSwitchState;
    statusData.rudder_switch = mState.mSwitches.mRudderSwitchState;
    statusData.test_button = mState.mSwitches.mTestSwitchState;
    statusData.memory_usage = mState.mSystem.mMemoryUsage;
    statusData.power_voltage = mState.mSystem.mPowerVoltage;
    statusData.uptime = mState.mSystem.mUptime;
    outlen = sizeof(statusData);
    return true;
}

bool UARTCommunication::resetDevice(const uint8_t &in, uint8_t &out, size_t &outlen) {
    LOG << "Reset device";
    mState.mActionRequest.mDeviceReset = true;
    outlen = 0;
    return true;
}

bool UARTCommunication::sendChannelSettings(const uint8_t &in, uint8_t &out, size_t &outlen) {
    const ChannelSettingsRequest &inReq = reinterpret_cast<const ChannelSettingsRequest&>(in);
    ChannelSettingsResponse &outRes = reinterpret_cast<ChannelSettingsResponse&>(out);

    if(inReq.channel_id >= 6) {
        return false;
    }
    outlen = sizeof(outRes);
    outRes.channel_id = inReq.channel_id;
    outRes.bridge = mState.mChannelSettings[inReq.channel_id].bridge;
    outRes.bridge_channel = mState.mChannelSettings[inReq.channel_id].bridge_channel;
    outRes.enable = mState.mChannelSettings[inReq.channel_id].enable;
    outRes.inverse_down_limit_switch = mState.mChannelSettings[inReq.channel_id].inverse_down_limit_switch;
    outRes.inverse_limit_switch = mState.mChannelSettings[inReq.channel_id].inverse_limit_switch;
    outRes.inverse_motor = mState.mChannelSettings[inReq.channel_id].inverse_motor;
    outRes.inverse_up_limit_switch = mState.mChannelSettings[inReq.channel_id].inverse_up_limit_switch;
    outRes.max_current_error_limit = mState.mChannelSettings[inReq.channel_id].max_current_error_limit;
    outRes.max_current_warning_limit = mState.mChannelSettings[inReq.channel_id].max_current_warning_limit;
    outRes.min_current_limit = mState.mChannelSettings[inReq.channel_id].min_current_limit;
    outRes.rudder = mState.mChannelSettings[inReq.channel_id].rudder;
    outRes.timeout = mState.mChannelSettings[inReq.channel_id].timeout; 
    return true;
}

bool UARTCommunication::updateChannelSettings(const uint8_t &in, uint8_t &out, size_t &outlen) {
    const ChannelSettingsResponse &data = reinterpret_cast<const ChannelSettingsResponse&>(in);

    if(data.channel_id >= 6) {
        return false;
    }
    
    mState.mChannelSettings[data.channel_id].bridge = data.bridge;
    mState.mChannelSettings[data.channel_id].bridge_channel = data.bridge_channel;
    mState.mChannelSettings[data.channel_id].enable = data.enable;
    mState.mChannelSettings[data.channel_id].inverse_down_limit_switch = data.inverse_down_limit_switch;
    mState.mChannelSettings[data.channel_id].inverse_limit_switch = data.inverse_limit_switch;
    mState.mChannelSettings[data.channel_id].inverse_motor = data.inverse_motor;
    mState.mChannelSettings[data.channel_id].inverse_up_limit_switch = data.inverse_up_limit_switch;
    mState.mChannelSettings[data.channel_id].max_current_error_limit = data.max_current_error_limit;
    mState.mChannelSettings[data.channel_id].max_current_warning_limit = data.max_current_warning_limit;
    mState.mChannelSettings[data.channel_id].min_current_limit = data.min_current_limit;
    mState.mChannelSettings[data.channel_id].rudder = data.rudder;
    mState.mChannelSettings[data.channel_id].timeout = data.timeout;
    mState.mActionRequest.mSaveSettings = true;
    return true;
}

bool UARTCommunication::sendMonitoringData(const uint8_t &in, uint8_t &out, size_t &outlen) {
    const MonitoringDataRequest &inReq = reinterpret_cast<const MonitoringDataRequest&>(in);
    MonitoringDataResponse &outRes = reinterpret_cast<MonitoringDataResponse&>(out);

    if(inReq.channel_id >= 6) {
        return false;
    }
    // Prepare response
    outRes.channel = inReq.channel_id;
    outRes.timestamp = mState.mMonitoringData[inReq.channel_id].timestamp;
    outRes.current = mState.mMonitoringData[inReq.channel_id].current;
    outRes.state = mState.mMonitoringData[inReq.channel_id].state;
    outRes.switches = mState.mMonitoringData[inReq.channel_id].switches;
    outlen = sizeof(outRes);
    return true;
}

bool UARTCommunication::remoteControl(const uint8_t &in, uint8_t &out, size_t &outlen) {
    const RemoteControlData &inReq = reinterpret_cast<const RemoteControlData&>(in);

    mState.mRemoteControl.mLdgGearSwitchState = inReq.ldg_gear_switch;
    mState.mRemoteControl.mRudderSwitchState = inReq.rudder_switch;
    mState.mRemoteControl.mTestSwitchState = inReq.test_button;
    mState.mRemoteControl.mSimulationTimeout = mState.mSystem.mUptime;

    outlen = 0;

    return true;
}

bool UARTCommunication::sendErrors(const uint8_t &in, uint8_t &out, size_t &outlen) {
    LOG << "Getting errors";
    ErrorStatus &outError = reinterpret_cast<ErrorStatus&>(out);
    for(int i=0; i<6; i++) {
        outError.errors[i] = mState.mErrors.getChannelErrors(i);
        outError.warnings[i] = mState.mErrors.getChannelWarnings(i);
    }
    outError.system_warnings = mState.mErrors.getSystemWarnings();
    outlen = sizeof(outError);
    return true;
}

bool UARTCommunication::spin() {
    // Process UART communication if available
    bool ret=false;
    char inBuff[255] = {};
    char outBuff[255] = {};

    while(UartStream::getInstance()->readLine(inBuff, sizeof(inBuff), 0)) {
        ret = true;
        if (mProtocol.process(inBuff, outBuff, sizeof(outBuff))) {
            Logger() << outBuff;
        }
    }
    return ret;
}

#pragma GCC pop_options
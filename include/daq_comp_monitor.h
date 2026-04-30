//
// Created by Jon Sensenig on 10/01/25.
//

#ifndef DAQ_COMP_MONITOR_H
#define DAQ_COMP_MONITOR_H

#include "metric_base.h"

using namespace constants::daq_computer;

class DaqCompMonitor : public MetricBase {
private:
    uint32_t error_bit_word_;
    uint32_t last_command_;
    uint32_t last_command_status_;
    uint32_t daq_bit_word_;
    uint32_t tpc_disk_;
    uint32_t tof_disk_;
    uint32_t sys_disk_;
    uint32_t cpu_usage_;
    uint32_t memory_usage_;
    uint32_t disk_temp_;
    std::array<uint32_t, NUM_CPUS> cpu_temp_;

    // Implement  the serialize/deserialize
    size_t num_members_ = 10;
    auto member_tuple() {
        return std::tie(error_bit_word_, last_command_, last_command_status_, daq_bit_word_, tpc_disk_,
                        tof_disk_, sys_disk_, cpu_usage_, memory_usage_, disk_temp_);
    };
    auto member_tuple() const {
        return std::tie(error_bit_word_, last_command_, last_command_status_, daq_bit_word_, tpc_disk_,
                        tof_disk_, sys_disk_, cpu_usage_, memory_usage_, disk_temp_);
    };

public:
    DaqCompMonitor();

    // Assign errors to the bits in the error word
    enum ErrorBits : uint32_t {
        cpu_temp_status = 1,
        disk_temp_status = 2,
        disk_free_status = 3,
        memory_usage_status = 4,
        cpu_usage_status = 5,
        start_tpc_daq = 6,
        stop_tpc_daq = 7,
        start_tof_daq = 8,
        stop_tof_daq = 9,
        start_tpc_dm = 10,
        stop_tpc_dm = 11,
        start_tof_dm = 12,
        stop_tof_dm = 13,
        init_pcie = 14,
        reboot_computer = 15,
        shutdown_computer = 16
    };

    // Each bit indicates if a DAQ is running or not
    enum DaqRunningBits : uint32_t {
        tpc = 0,
        tpc_monitor = 1,
        tof = 2,
        tof_monitor = 3
    };

    void clear();
    void print() const;

    // Public setters for populating data
    void setLastCommand(uint32_t last_command) { last_command_ = last_command; }
    void setLastCommandStatus(uint32_t last_command_status) { last_command_status_ = last_command_status; }
    void setTpcDisk(uint32_t disk) { tpc_disk_ = disk; }
    void setTofDisk(uint32_t disk) { tof_disk_ = disk; }
    void setSysDisk(uint32_t disk) { sys_disk_ = disk; }
    void setCpuUsage(uint32_t usage) { cpu_usage_ = usage; }
    void setMemoryUsage(uint32_t usage) { memory_usage_ = usage; }
    void setDiskTemp(uint32_t temp) { disk_temp_ = temp; }
    void setCpuTemp(std::array<uint32_t, NUM_CPUS> temp_vec) { cpu_temp_ = temp_vec; }

    // Set Error or DAQ process bit words
    void setDaqBitWord(DaqRunningBits daq_bit, bool unset=false) { setBitWord(daq_bit_word_, to_underlying(daq_bit), unset); }
    void setErrorBitWord(ErrorBits error_bit, bool unset=false) { setBitWord(error_bit_word_, to_underlying(error_bit), unset); }
    void clearErrorBitWord() { error_bit_word_ = 0; }

    // --- Getter Methods ---
    // Public setters for populating data
    uint32_t getFullErrorBitWord() const { return error_bit_word_; }
    uint32_t getLastCommand() const { return last_command_; }
    uint32_t getLastCommandStatus() const { return last_command_status_; }
    uint32_t getFullDaqBitWord() const { return daq_bit_word_; }
    uint32_t getTpcDisk() const { return tpc_disk_; }
    uint32_t getTofDisk() const { return tof_disk_; }
    uint32_t getSysDisk() const { return sys_disk_; }
    uint32_t getCpuUsage() const { return cpu_usage_; }
    uint32_t getMemoryUsage() const { return memory_usage_; }
    uint32_t getDiskTemp() const { return disk_temp_; }
    const std::array<uint32_t, NUM_CPUS> getCpuTemp() const { return cpu_temp_; }

    // MetricBase interface implementation
    std::vector<uint32_t> serialize() const override;
    std::vector<uint32_t>::const_iterator deserialize(std::vector<uint32_t>::const_iterator begin,
                                                     std::vector<uint32_t>::const_iterator end) override;

#ifdef USE_PYTHON
    py::dict getMetricDict() override;
#endif

};

#endif //DAQ_COMP_MONITOR_H

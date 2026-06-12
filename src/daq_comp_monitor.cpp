//
// Created by Jon Sensenig on 10/01/25.
//

#include "../include/daq_comp_monitor.h"

#include <bitset>
#include <stdexcept>
#include <iostream>

DaqCompMonitor::DaqCompMonitor() : error_bit_word_(0), last_command_(0), last_command_status_(0), daq_bit_word_(0),
                                   tpc_disk_0_(0), tof_disk_0_(0), tpc_disk_1_(0), tof_disk_1_(0), sys_disk_(0), 
                                   cpu_usage_(0), memory_usage_(0), disk_temp_(0) {
    std::fill(cpu_temp_.begin(), cpu_temp_.end(), 0);
}

void DaqCompMonitor::clear() {
    error_bit_word_ = 0;
    last_command_ = 0;
    last_command_status_ = 0;
    daq_bit_word_ = 0;
    tpc_disk_0_ = 0;
    tof_disk_0_ = 0;
    tpc_disk_1_ = 0;
    tof_disk_1_ = 0;
    sys_disk_ = 0;
    cpu_usage_ = 0;
    memory_usage_ = 0;
    disk_temp_ = 0;
    std::fill(cpu_temp_.begin(), cpu_temp_.end(), 0);
}

std::vector<uint32_t> DaqCompMonitor::serialize() const {
    std::vector<uint32_t> serialized_data;
    // Reserve space for efficiency
    serialized_data.reserve(num_members_ + NUM_CPUS);

    // Serialize the histogram metadata
    auto data = Serializer<DaqCompMonitor>::serialize_tuple(member_tuple());
    serialized_data.insert(serialized_data.end(), data.begin(), data.end());
    // Charge channel number of samples data
    serialized_data.insert(serialized_data.end(), cpu_temp_.begin(), cpu_temp_.end());
    return serialized_data;
}

std::vector<uint32_t>::const_iterator DaqCompMonitor::deserialize(std::vector<uint32_t>::const_iterator begin,
                                                            std::vector<uint32_t>::const_iterator end) {

    auto it = begin;
    it = Serializer<DaqCompMonitor>::deserialize_tuple(member_tuple(), begin, end);

    // Ensure there's enough data for the bins
    if (static_cast<size_t>(std::distance(it, end)) < NUM_CPUS) {
        throw std::runtime_error("Deserialization failed: not enough data for number CPUs.");
    }

    // Copy num charge samples data
    std::copy(it, it + NUM_CPUS, cpu_temp_.begin());
    it += NUM_CPUS;

    return it;
}

#ifdef USE_PYTHON
py::dict DaqCompMonitor::getMetricDict() {

    py::dict metric_dict;
    metric_dict["error_bit_word"] = error_bit_word_;
    metric_dict["last_command"] = last_command_;
    metric_dict["last_command_status"] = last_command_status_;
    metric_dict["daq_bit_word"] = daq_bit_word_;
    metric_dict["tpc_disk_0"] = tpc_disk_0_;
    metric_dict["tof_disk_0"] = tof_disk_0_;
    metric_dict["tpc_disk_1"] = tpc_disk_1_;
    metric_dict["tof_disk_1"] = tof_disk_1_;
    metric_dict["sys_disk"] = sys_disk_;
    metric_dict["cpu_usage"] = cpu_usage_;
    metric_dict["memory_usage"] = memory_usage_;
    metric_dict["disk_temp"] = disk_temp_;
    metric_dict["cpu_temp"] = array_to_numpy_array_1d(cpu_temp_);

    return metric_dict;
}
#endif

void DaqCompMonitor::print() const {
    std::cout << "++++++++++++ DaqCompMonitor +++++++++++++" << std::endl;
    std::cout << "  error_bit_word: " << std::bitset<32>(error_bit_word_) << std::endl;
    std::cout << "  last_command: " << std::bitset<32>(last_command_) << std::endl;
    std::cout << "  last_command_status: " << std::bitset<32>(last_command_status_) << std::endl;
    std::cout << "  daq_bit_word: " << std::bitset<32>(daq_bit_word_) << std::endl;
    std::cout << "  tpc_disk0: " << tpc_disk_0_ << std::endl;
    std::cout << "  tpc_disk1: " << tpc_disk_1_ << std::endl;
    std::cout << "  tof_disk0: " << tof_disk_0_ << std::endl;
    std::cout << "  tof_disk1: " << tof_disk_1_ << std::endl;
    std::cout << "  sys_disk: " << sys_disk_ << std::endl;
    std::cout << "  cpu_usage: " << cpu_usage_ << std::endl;
    std::cout << "  memory_usage: " << memory_usage_ << std::endl;
    std::cout << "  disk_temp: " << disk_temp_ << std::endl;
    std::cout << "  CPU Temp.: ";
    for (size_t i = 0; i < 10 && i < cpu_temp_.size(); ++i) {
        std::cout << cpu_temp_[i] << (i == 9 || i == cpu_temp_.size() - 1 ? "" : ", ");
    }
    std::cout << "..." << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
}

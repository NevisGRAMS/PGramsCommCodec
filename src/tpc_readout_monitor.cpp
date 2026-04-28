//
// Created by Jon Sensenig on 8/23/25.
//

#include "../include/tpc_readout_monitor.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

TpcReadoutMonitor::TpcReadoutMonitor() : error_bit_word_(0), num_rw_buffer_overflow_(0), readout_state_(0),
    last_command_(0), last_command_status_(0), run_number_(0), num_events_upper_(0),
    num_events_lower_(0), num_dma_loops_upper_(0),
    num_dma_loops_lower_(0), received_mbytes_upper_(0), received_mbytes_lower_(0), avg_event_size_upper_(0),
    avg_event_size_lower_(0), num_files_upper_(0), num_files_lower_(0), num_event_start_marker_upper_(0),
    num_event_start_marker_lower_(0), num_event_end_marker_upper_(0), num_event_end_marker_lower_(0), pps_count_(0) {
    std::fill(board_status_.begin(), board_status_.end(), 0);
}

void TpcReadoutMonitor::clear() {
    error_bit_word_ = 0;
    num_rw_buffer_overflow_ = 0;
    readout_state_ = 0;
    last_command_ = 0;
    last_command_status_ = 0;
    run_number_ = 0;
    num_events_upper_ = 0;
    num_events_lower_ = 0;
    // event_diff_upper_ = 0;
    // event_diff_lower_ = 0;
    num_dma_loops_upper_ = 0;
    num_dma_loops_lower_ = 0;
    received_mbytes_upper_ = 0;
    received_mbytes_lower_ = 0;
    avg_event_size_upper_ = 0;
    avg_event_size_lower_ = 0;
    num_files_upper_ = 0;
    num_files_lower_ = 0;
    num_event_start_marker_upper_ = 0;
    num_event_start_marker_lower_ = 0;
    num_event_end_marker_upper_ = 0;
    num_event_end_marker_lower_ = 0;
    pps_count_ = 0;
    std::fill(board_status_.begin(), board_status_.end(), 0);
}

std::vector<uint32_t> TpcReadoutMonitor::serialize() const {
    std::vector<uint32_t> serialized_data;
    // Reserve space for efficiency
    serialized_data.reserve(num_members_ + NUM_BOARDS);

    // Serialize the histogram metadata
    auto data = Serializer<TpcReadoutMonitor>::serialize_tuple(member_tuple());
    serialized_data.insert(serialized_data.end(), data.begin(), data.end());
    // Charge channel number of samples data
    serialized_data.insert(serialized_data.end(), board_status_.begin(), board_status_.end());
    return serialized_data;
}

std::vector<uint32_t>::const_iterator TpcReadoutMonitor::deserialize(std::vector<uint32_t>::const_iterator begin,
                                                            std::vector<uint32_t>::const_iterator end) {

    auto it = begin;
    it = Serializer<TpcReadoutMonitor>::deserialize_tuple(member_tuple(), begin, end);

    // Ensure there's enough data for the bins
    if (static_cast<size_t>(std::distance(it, end)) < NUM_BOARDS) {
        std::ostringstream oss;
        oss << "Deserialization failed: not enough data for Board Status " << static_cast<size_t>(std::distance(it, end));
        throw std::runtime_error(oss.str());
    }

    // Copy num charge samples data
    std::copy(it, it + NUM_BOARDS, board_status_.begin());
    it += NUM_BOARDS;

    return it;
}

#ifdef USE_PYTHON
py::dict TpcReadoutMonitor::getMetricDict() {

    py::dict metric_dict;
    metric_dict["error_bit_word"] = error_bit_word_;
    metric_dict["num_rw_buffer_overflow"] = num_rw_buffer_overflow_;
    metric_dict["readout_state"] = readout_state_;
    metric_dict["last_command"] = last_command_;
    metric_dict["last_command_status"] = last_command_status_;
    metric_dict["run_number"] = run_number_;
    metric_dict["num_events"] = getFullWord(num_events_upper_, num_events_lower_);
    // metric_dict["event_diff"] = getFullWord(event_diff_upper_, event_diff_lower_);
    metric_dict["num_dma_loops"] = getFullWord(num_dma_loops_upper_, num_dma_loops_lower_);
    metric_dict["received_mbytes"] = getFullWord(received_mbytes_upper_, received_mbytes_lower_);
    metric_dict["avg_event_size"] = getFullWord(avg_event_size_upper_, avg_event_size_lower_);
    metric_dict["num_files"] = getFullWord(num_files_upper_, num_files_lower_);
    metric_dict["num_start_markers"] = getFullWord(num_event_start_marker_upper_, num_event_start_marker_lower_);
    metric_dict["num_end_markers"] = getFullWord(num_event_end_marker_upper_, num_event_end_marker_lower_);
    metric_dict["pps_count"] = pps_count_;
    metric_dict["board_status"] = array_to_numpy_array_1d(board_status_);

    return metric_dict;
}
#endif

void TpcReadoutMonitor::print() const {
    std::cout << "++++++++++++ TpcReadoutMonitor +++++++++++++" << std::endl;
    std::cout << "  error_bit_word: " << error_bit_word_ << std::endl;
    std::cout << "  num_rw_buffer_overflow: " << num_rw_buffer_overflow_ << std::endl;
    std::cout << "  readout_state: " << readout_state_ << std::endl;
    std::cout << "  last_command: " << last_command_ << std::endl;
    std::cout << "  last_command_status: " << last_command_status_ << std::endl;
    std::cout << "  run_number: " << run_number_ << std::endl;
    std::cout << "  num_events: " << getFullWord(num_events_upper_, num_events_lower_) << std::endl;
    // std::cout << "  event_diff: " << getFullWord(event_diff_upper_, event_diff_lower_) << std::endl;
    std::cout << "  num_dma_loops: " << getFullWord(num_dma_loops_upper_, num_dma_loops_lower_) << std::endl;
    std::cout << "  received_mbytes: " << getFullWord(received_mbytes_upper_, received_mbytes_lower_) << std::endl;
    std::cout << "  avg_event_size: " << getFullWord(avg_event_size_upper_, avg_event_size_lower_) << std::endl;
    std::cout << "  num_files: " << getFullWord(num_files_upper_, num_files_lower_) << std::endl;
    std::cout << "  num_start_markers: " << getFullWord(num_event_start_marker_upper_, num_event_start_marker_lower_) << std::endl;
    std::cout << "  num_end_markers: " << getFullWord(num_event_end_marker_upper_, num_event_end_marker_lower_) << std::endl;
    std::cout << "  pps_count: " << pps_count_ << std::endl;
    std::cout << "  Board Status: " << std::hex;
    for (size_t i = 0; i < board_status_.size(); ++i) {
        std::cout << board_status_[i] << (i == board_status_.size() - 1 ? "" : ", ");
    }
    std::cout << std::dec << "..." << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
}

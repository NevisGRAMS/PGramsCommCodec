//
// Created by Jon Sensenig on 10/07/25.
//

#ifndef TPC_READOUT_MONITOR_H
#define TPC_READOUT_MONITOR_H

#include "metric_base.h"

using namespace constants::tpc_readout;

class TpcReadoutMonitor : public MetricBase {
private:
    uint32_t error_bit_word_;
    uint32_t num_rw_buffer_overflow_;
    uint32_t readout_state_;
    uint32_t last_command_;
    uint32_t last_command_status_;
    uint32_t run_number_;

    uint32_t num_events_upper_;
    uint32_t num_events_lower_;

    // uint32_t event_diff_upper_;
    // uint32_t event_diff_lower_;

    uint32_t num_dma_loops_upper_;
    uint32_t num_dma_loops_lower_;

    uint32_t received_mbytes_upper_;
    uint32_t received_mbytes_lower_;

    uint32_t avg_event_size_upper_;
    uint32_t avg_event_size_lower_;

    uint32_t num_files_upper_;
    uint32_t num_files_lower_;

    uint32_t num_event_start_marker_upper_;
    uint32_t num_event_start_marker_lower_;

    uint32_t num_event_end_marker_upper_;
    uint32_t num_event_end_marker_lower_;

    uint32_t pps_count_;

    std::array<uint32_t, NUM_BOARDS> board_status_;

    enum class State : int {
        kIdle = 0,
        kConfigured = 1,
        kRunning = 2,
        kStopped = 3
      };

    // Implement  the serialize/deserialize
    size_t num_members_ = 21;
    auto member_tuple() {
        return std::tie(error_bit_word_, num_rw_buffer_overflow_, readout_state_, last_command_, last_command_status_,
                      run_number_, num_events_upper_, num_events_lower_,
                      num_dma_loops_upper_, num_dma_loops_lower_, received_mbytes_upper_,
                      received_mbytes_lower_, avg_event_size_upper_, avg_event_size_lower_, num_files_upper_,
                      num_files_lower_, num_event_start_marker_upper_, num_event_start_marker_lower_,
                      num_event_end_marker_upper_, num_event_end_marker_lower_, pps_count_);
    };
    auto member_tuple() const {
        return std::tie(error_bit_word_, num_rw_buffer_overflow_, readout_state_, last_command_, last_command_status_,
                        run_number_, num_events_upper_, num_events_lower_,
                        num_dma_loops_upper_, num_dma_loops_lower_, received_mbytes_upper_,
                        received_mbytes_lower_, avg_event_size_upper_, avg_event_size_lower_, num_files_upper_,
                        num_files_lower_, num_event_start_marker_upper_, num_event_start_marker_lower_,
                        num_event_end_marker_upper_, num_event_end_marker_lower_, pps_count_);
    };

    static inline uint32_t getUpper32(size_t word) { return (word >> 32) & UINT32_MAX; }
    static inline uint32_t getLower32(size_t word) { return word & UINT32_MAX; }
    static inline size_t getFullWord(size_t upper_word, size_t lower_word) { return (upper_word << 32) + lower_word; }

public:
    TpcReadoutMonitor();

    // Assign errors to the bits in the error word
    enum ErrorBits : uint32_t {
        daq_state_cmd = 1,
        pcie_license = 2,
        config_load = 3,
        comms_config_load = 4,
        // Do not change these
        pcie_lib_init = 5,
        pcie_card_open = 6,
        pcie_control_buff = 7,
        xmit_get_config = 8,
        lightfem_get_config = 9,
        chargefem_get_config = 10,
        trigger_get_config = 11,
        datahandler_get_config = 12,
        acquire_pcie_dma_buff = 13
    };

    void setErrorBitWord(ErrorBits error_bit, bool unset=false) { setBitWord(error_bit_word_, to_underlying(error_bit), unset); }
    void setErrorBitWord(uint32_t error_bit, bool unset=false) { setBitWord(error_bit_word_, error_bit, unset); }
    uint32_t getErrorBit(uint32_t err_bit) const { return getBit(error_bit_word_, err_bit); }

    void clear();
    void print() const;

    // Public setters for populating data
    void setNumRwBufferOverflow(uint32_t count) { num_rw_buffer_overflow_ = count; }
    void setReadoutState(uint32_t state) { readout_state_ = state; };
    void setLastCommand(uint32_t command) { last_command_ = command; };
    void setLastCommandStatus(uint32_t command_status) { last_command_status_ = command_status; };
    void setRunNumber(uint32_t run_number) { run_number_ = run_number; };
    void setNumEvents(size_t num_events) {
      num_events_upper_ = getUpper32(num_events);
      num_events_lower_ = getLower32(num_events);
    };
    // void setEventDiff(size_t event_diff) {
    //     event_diff_upper_ = getUpper32(event_diff);
    //     event_diff_lower_ = getLower32(event_diff);
    // };
    void setNumDmaLoops(size_t dma_loops) {
        num_dma_loops_upper_ = getUpper32(dma_loops);
        num_dma_loops_lower_ = getLower32(dma_loops);
    };
     void setReceivedMbytes(size_t received_mbytes) {
       received_mbytes_upper_ = getUpper32(received_mbytes);
       received_mbytes_lower_ = getLower32(received_mbytes);
     }
     void setAvgEventSize(size_t avg_event_size) {
       avg_event_size_upper_ = getUpper32(avg_event_size);
       avg_event_size_lower_ = getLower32(avg_event_size);
     }
     void setNumFiles(size_t num_files) {
       num_files_upper_ = getUpper32(num_files);
       num_files_lower_ = getLower32(num_files);
     }
    void setStartMarker(size_t start_marker) {
         num_event_start_marker_upper_ = getUpper32(start_marker);
         num_event_start_marker_lower_ = getLower32(start_marker);
     }
    void setEndMarker(size_t end_marker) {
         num_event_end_marker_upper_ = getUpper32(end_marker);
         num_event_end_marker_lower_ = getLower32(end_marker);
     }
    void setPpsCount(uint32_t pps_count) { pps_count_ = pps_count; }
    void setBoardStatus(std::vector<uint32_t> &board_status) {
         for (size_t i = 0; i < board_status.size(); i++) {
             board_status_.at(i) = board_status.at(i);
         }
     }

    // --- Getter Methods ---
    uint32_t getNumRwBufferOverflow() const { return num_rw_buffer_overflow_; };
    uint32_t getReadoutState() const { return readout_state_; }
    uint32_t getLastCommand() const { return last_command_; }
    uint32_t getLastCommandStatus() const { return last_command_status_; }
    uint32_t getRunNumber() const { return run_number_; }
    size_t getNumEvents() const { return getFullWord(num_events_upper_, num_events_lower_); }
    // size_t getEventDiff() const { return getFullWord(num_events_upper_, num_events_lower_); }
    size_t getNumDmaLoops() const { return getFullWord(num_dma_loops_upper_, num_dma_loops_lower_); }
    size_t getReceivedMbytes() const { return getFullWord(received_mbytes_upper_, received_mbytes_lower_); }
    size_t getAvgEventSize() const { return getFullWord(avg_event_size_upper_, avg_event_size_lower_); }
    size_t getNumFiles() const { return getFullWord(num_files_upper_, num_files_lower_); }
    size_t getNumStartMarkers() const { return getFullWord(num_event_start_marker_upper_, num_event_start_marker_lower_); }
    size_t getNumEndMarkers() const { return getFullWord(num_event_end_marker_upper_, num_event_end_marker_lower_); }
    uint32_t getPpsCount() const { return pps_count_; }
    const std::array<uint32_t, NUM_BOARDS> getBoardStatus() const { return board_status_; }

    // MetricBase interface implementation
    std::vector<uint32_t> serialize() const override;
    std::vector<uint32_t>::const_iterator deserialize(std::vector<uint32_t>::const_iterator begin,
                                                     std::vector<uint32_t>::const_iterator end) override;

#ifdef USE_PYTHON
    py::dict getMetricDict() override;
#endif

};

#endif //TPC_READOUT_MONITOR_H

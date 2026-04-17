//
// Created by Jon Sensenig on 12/9/25.
//

#ifndef TPC_MONITOR_LIGHT_EVENTS_H
#define TPC_MONITOR_LIGHT_EVENT_H

#include "metric_base.h"

using namespace constants::tpc_readout;

class TpcMonitorLightEvent : public MetricBase {

private:

    uint32_t channel_number_;
    uint32_t run_number_;
    uint32_t file_number_;
    uint32_t evt_number_;
    uint32_t num_samples_;
    uint32_t start_tick_; // 2Mhz clock
    std::vector<uint32_t> light_samples_{};

    // Implement  the serialize/deserialize
    size_t num_members_ = 5;
    auto member_tuple() {
        return std::tie(channel_number_, run_number_, file_number_, evt_number_, num_samples_, start_tick_);
    };
    auto member_tuple() const {
        return std::tie(channel_number_, run_number_, file_number_, evt_number_, num_samples_, start_tick_);
    };

public:

    TpcMonitorLightEvent();
    void clear();
    void print();

    void PackDoubleWords(std::vector<uint32_t> &source_array, std::vector<uint32_t> &dest_array) {
        // Packing two 16b per 32b words with i+1 in upper and i in the lower bits
        for (size_t i = 0; i < dest_array.size(); i++) {
            dest_array[i] = ((source_array[2 * i + 1]  & 0xFFFF) << 16) + (source_array[2 * i] & 0xFFFF);
        }
    }

    std::vector<uint32_t> UnPackDoubleWords(std::vector<uint32_t> &source_array) {
        // Unpacking two 16b per 32b words with i+1 in upper and i in the lower bits
        std::vector<uint32_t> dest_array;
        dest_array.resize(2 * source_array.size());
        for (size_t i = 0; i < source_array.size(); i++) {
            dest_array[2 * i] = source_array[i] & 0xFFFF;
            dest_array[2 * i + 1] = (source_array[i] >> 16) & 0xFFFF;
        }
        return dest_array;
    }

    void setChannelNumber(uint32_t channel_number) { channel_number_ = channel_number; }
    void setRunNumber(uint32_t run_number) { run_number_ = run_number; }
    void setFileNumber(uint32_t file_number) { file_number_ = file_number; }
    void setEvtNumber(uint32_t evt_number) { evt_number_ = evt_number; }
    void setStartTick(uint32_t start_tick) { start_tick_ = start_tick; }
    void setLightSamples(std::vector<uint32_t> &light_roi) {
        num_samples_ = (light_roi.size() + 1) / 2;
        light_samples_.resize(num_samples_);
        PackDoubleWords(light_roi, light_samples_);
    }

    // --- Getter Methods ---
    const uint32_t getChannelNumber() const { return channel_number_; }
    const uint32_t getRunNumber() const { return run_number_; }
    const uint32_t getFileNumber() const { return file_number_; }
    const uint32_t getEvtNumber() const { return evt_number_; }
    const uint32_t getStartTick() const { return start_tick_; }
    const uint32_t getNumSamples() const { return num_samples_; }
    const std::vector<uint32_t>& getLightSamples() const { return light_samples_; }

    // MetricBase interface implementation
    std::vector<uint32_t> serialize() const override;
    std::vector<uint32_t>::const_iterator deserialize(std::vector<uint32_t>::const_iterator begin,
                                                     std::vector<uint32_t>::const_iterator end) override;

#ifdef USE_PYTHON
    py::dict getMetricDict() override;
#endif

};
#endif //TPC_MONITOR_LIGHT_EVENTS_H

//
// Created by Jon Sensenig on 9/4/25.
//

#include "../include/tpc_configs.h"
#include <stdexcept>
#include <iostream>
#include <algorithm> // for std::fill, std::copy

TpcConfigs::TpcConfigs()
    : config_file_number_(0),
      summed_peak_thresh_(0),
      channel_multiplicity_(0),
      roi_delay_0_(0),
      roi_delay_1_(0),
      roi_precount_(0),
      roi_peak_window_(0),
      enable_top_(0xFF),
      enable_middle_(0xFFFF),
      enable_bottom_(0xFFFF),
      num_roi_words_(256),
      roi_deadtime_(240),
      fifo_blocksize_(0xFFFF),
      drift_size_(0),
      trigger_source_(1),
      software_trigger_rate_hz_(50),
      tpc_dead_time_(0x10),
      light_trig_prescale_(0),
      unbiased_light_samples_(0), 
      dma_buffer_kB_(0) {
    // Fill the arrays with default values
    std::fill(prescale_.begin(), prescale_.end(), 0);
    std::fill(disc_threshold_0_.begin(), disc_threshold_0_.end(), 100);
    std::fill(disc_threshold_1_.begin(), disc_threshold_1_.end(), 200);
    std::fill(disc_threshold_3_.begin(), disc_threshold_3_.end(), 0xFFF);
    std::fill(disc_threshold_4_.begin(), disc_threshold_4_.end(), 0xFFF);
}

void TpcConfigs::clear() {

    summed_peak_thresh_ = 0;
    channel_multiplicity_ = 0;
    roi_delay_0_ = 0;
    roi_delay_1_ = 0;
    roi_precount_ = 0;
    roi_peak_window_ = 0;
    enable_top_ = 0x0;
    enable_middle_ = 0x0;
    enable_bottom_ = 0x0;
    num_roi_words_ = 0;
    roi_deadtime_ = 0;
    fifo_blocksize_ = 0xFFFF;
    drift_size_ = 0;
    trigger_source_ = 1;
    software_trigger_rate_hz_ = 50;
    tpc_dead_time_ = 0x10;
    light_trig_prescale_ = 0;
    unbiased_light_samples_ = 0;
    dma_buffer_kB_ = 0;
    std::fill(prescale_.begin(), prescale_.end(), 0);
    std::fill(disc_threshold_0_.begin(), disc_threshold_0_.end(), 0);
    std::fill(disc_threshold_1_.begin(), disc_threshold_1_.end(), 0);

    std::fill(disc_threshold_3_.begin(), disc_threshold_3_.end(), 0xFFF);
    std::fill(disc_threshold_4_.begin(), disc_threshold_4_.end(), 0xFFF);

}

std::vector<uint32_t> TpcConfigs::serialize() const {
    std::vector<uint32_t> serialized_data;
    // Reserve space for efficiency
    serialized_data.reserve(num_members_ + prescale_.size() + disc_threshold_0_.size() + disc_threshold_1_.size());

    // Serialize the histogram metadata
    auto data = Serializer<TpcConfigs>::serialize_tuple(member_tuple());
    serialized_data.insert(serialized_data.end(), data.begin(), data.end());

    // Trigger prescales
    serialized_data.insert(serialized_data.end(), prescale_.begin(), prescale_.end());
    // Disc channel thresholds
    serialized_data.insert(serialized_data.end(), disc_threshold_0_.begin(), disc_threshold_0_.end());
    serialized_data.insert(serialized_data.end(), disc_threshold_1_.begin(), disc_threshold_1_.end());

    return serialized_data;
}

std::vector<uint32_t>::const_iterator TpcConfigs::deserialize(std::vector<uint32_t>::const_iterator begin,
                                                             std::vector<uint32_t>::const_iterator end) {
    auto it = begin;
    // Need scalars first
    if (static_cast<size_t>(std::distance(it, end)) < num_members_) {
        throw std::runtime_error("Deserialization failed: Words received [" +
            std::to_string(std::distance(it, end)) + "] but requires [" + std::to_string(num_members_) + "]");
    }
    it = Serializer<TpcConfigs>::deserialize_tuple(member_tuple(), begin, end);

    auto copy_array = [&](auto& arr) {
        using ArrayType = std::decay_t<decltype(arr)>;
        static_assert(std::is_same_v<typename ArrayType::value_type, uint32_t>, "Array element type must be uint32_t");

        if (static_cast<size_t>(std::distance(it, end)) < arr.size()) {
            throw std::runtime_error("Deserialization failed: not enough data for array.");
        }
        std::copy(it, it + arr.size(), arr.begin());
        it += arr.size();
    };

    copy_array(prescale_);
    copy_array(disc_threshold_0_);
    copy_array(disc_threshold_1_);

    return it;
}

#ifdef USE_PYTHON
py::dict TpcConfigs::getMetricDict() {
    py::dict metric_dict;

    metric_dict["config_file_number"] = config_file_number_;
    metric_dict["summed_peak_thresh"] = summed_peak_thresh_;
    metric_dict["channel_multiplicity"] = channel_multiplicity_;
    metric_dict["roi_delay_0"] = roi_delay_0_;
    metric_dict["roi_delay_1"] = roi_delay_1_;
    metric_dict["roi_precount"] = roi_precount_;
    metric_dict["roi_peak_window"] = roi_peak_window_;
    metric_dict["enable_top"] = enable_top_;
    metric_dict["enable_middle"] = enable_middle_;
    metric_dict["enable_bottom"] = enable_bottom_;
    metric_dict["num_roi_words"] = num_roi_words_;
    metric_dict["roi_deadtime"] = roi_deadtime_;
    metric_dict["fifo_blocksize"] = fifo_blocksize_;
    metric_dict["drift_size"] = drift_size_;
    metric_dict["trigger_source"] = trigger_source_;
    metric_dict["software_trigger_rate_hz"] = software_trigger_rate_hz_;
    metric_dict["tpc_dead_time"] = tpc_dead_time_;
    metric_dict["light_trig_prescale"] = light_trig_prescale_;
    metric_dict["unbiased_light_samples"] = unbiased_light_samples_;
    metric_dict["dma_buffer_kB"] = dma_buffer_kB_;

    metric_dict["prescale"] = array_to_numpy_array_1d(prescale_);
    metric_dict["disc_threshold_0"] = array_to_numpy_array_1d(disc_threshold_0_);
    metric_dict["disc_threshold_1"] = array_to_numpy_array_1d(disc_threshold_1_);

    return metric_dict;
}

void TpcConfigs::setMetricDict(py::dict &config) {

    AssignScalar(config_file_number_, config, "config_file_number");
    AssignScalar(summed_peak_thresh_, config, "summed_peak_thresh");
    AssignScalar(channel_multiplicity_, config, "channel_multiplicity");
    AssignScalar(roi_delay_0_, config, "roi_delay_0");
    AssignScalar(roi_delay_1_, config, "roi_delay_1");
    AssignScalar(roi_precount_, config, "roi_precount");
    AssignScalar(roi_peak_window_, config, "roi_peak_window");
    AssignScalar(enable_top_, config, "enable_top");
    AssignScalar(enable_middle_, config, "enable_middle");
    AssignScalar(enable_bottom_, config, "enable_bottom");
    AssignScalar(num_roi_words_, config, "num_roi_words");
    AssignScalar(roi_deadtime_, config, "roi_deadtime");
    AssignScalar(fifo_blocksize_, config, "fifo_blocksize");
    AssignScalar(drift_size_, config, "drift_size");
    AssignScalar(trigger_source_, config, "trigger_source");
    AssignScalar(software_trigger_rate_hz_, config, "software_trigger_rate_hz");
    AssignScalar(tpc_dead_time_, config, "tpc_dead_time");
    AssignScalar(light_trig_prescale_, config, "light_trig_prescale");
    AssignScalar(unbiased_light_samples_, config, "unbiased_light_samples");
    AssignScalar(dma_buffer_kB_, config, "dma_buffer_kB");

    AssignArray(prescale_, config, "prescale");
    AssignArray(disc_threshold_0_, config, "disc_threshold_0");
    AssignArray(disc_threshold_1_, config, "disc_threshold_1");

}
#endif

void TpcConfigs::print() const {
    std::cout << "++++++++++++ TpcConfigs +++++++++++++" << std::endl;
    std::cout << "  config_file_number: " << config_file_number_ << std::endl;
    std::cout << "  summed_peak_thresh: " << summed_peak_thresh_ << std::endl;
    std::cout << "  channel_multiplicity: " << channel_multiplicity_ << std::endl;
    std::cout << "  roi_delay_0: " << roi_delay_0_ << std::endl;
    std::cout << "  roi_delay_1: " << roi_delay_1_ << std::endl;
    std::cout << "  roi_precount: " << roi_precount_ << std::endl;
    std::cout << "  roi_peak_window: " << roi_peak_window_ << std::endl;
    std::cout << "  enable_top: " << enable_top_ << std::endl;
    std::cout << "  enable_middle: " << enable_middle_ << std::endl;
    std::cout << "  enable_bottom: " << enable_bottom_ << std::endl;
    std::cout << "  num_roi_words: " << num_roi_words_ << std::endl;
    std::cout << "  roi_deadtime: " << roi_deadtime_ << std::endl;
    std::cout << "  fifo_blocksize: " << fifo_blocksize_ << std::endl;
    std::cout << "  drift_size: " << drift_size_ << std::endl;
    std::cout << "  trigger_source: " << trigger_source_ << std::endl;
    std::cout << "  software_trigger_rate_hz: " << software_trigger_rate_hz_ << std::endl;
    std::cout << "  tpc_dead_time: " << tpc_dead_time_ << std::endl;
    std::cout << "  unbiased_light_samples: " << unbiased_light_samples_ << std::endl;
    std::cout << "  dma_buffer_kB: " << dma_buffer_kB_ << std::endl;

    auto print_vector = [&](const std::string& name, const auto& arr) {
        std::cout << "  " << name << " (first 10): ";
        for (size_t i = 0; i < 10 && i < arr.size(); ++i) {
            std::cout << arr[i] << (i == 9 || i == arr.size() - 1 ? "" : ", ");
        }
        std::cout << "..." << std::endl;
    };

    print_vector("disc_threshold_0", disc_threshold_0_);
    print_vector("disc_threshold_1", disc_threshold_1_);
    print_vector("prescale", prescale_);

    std::cout << "++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
}

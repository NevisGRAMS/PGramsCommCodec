//
// Created by Jon Sensenig on 8/23/25.
//

#ifndef TPC_CONFIGS_H
#define TPC_CONFIGS_H

#include "metric_base.h"
#include <vector>
#include <iostream>

using namespace constants::tpc_readout;

class TpcConfigs : public MetricBase {
private:

    // Configuration parameters ot be set from the ground

    // the on-Sabertooth config file to be loaded
    uint32_t config_file_number_;

    /************************************
     *  Light Trigger & ROI settings
    **************************************/

    // cosmic_summed_adc_thresh: Summed peak ampltitudes of 5 adjacent channels
    uint32_t summed_peak_thresh_;
    // cosmic_multiplicity: Number of 5 adjacent channels above Disc 1 threshold
    uint32_t channel_multiplicity_;
    // pmt_delay_0: Number of samples to shift the waveforms to perform the subtraction
    uint32_t roi_delay_0_;
    // pmt_delay_1: ?
    uint32_t roi_delay_1_;
    // pmt_precount: number of samples before disc. 0 threshold to include in the ROI
    uint32_t roi_precount_;
    // pmt_width: window where the waveform peak is found
    uint32_t roi_peak_window_;
    // sipm_words: Number of samples in the ROI
    uint32_t num_roi_words_;
    // sipm_deadtime: The number of samples after the ROI which are an enforced deadtime
    uint32_t roi_deadtime_;
    // pmt_blocksize: curerntly no known reason to change this so keep set to 0xFFFF
    uint32_t fifo_blocksize_ = 0xFFFF;
    // Both "pmt_gate_size" and  "pmt_beam_size" get set with this value
    //  The width (in 64MHz ticks) for the unbiased light readout
    uint32_t unbiased_light_samples_;

    /************************************
     *  Light Channel Enable/Disable
    **************************************/

    // pmt_enable_top: Enable mask for channels on top connector
    uint32_t enable_top_;
    // pmt_enable_middle: Enable mask for channels on middle connector
    uint32_t enable_middle_;
    // pmt_enable_bottom: Enable mask for channels on bottom connector
    uint32_t enable_bottom_;

    /************************************
    *  TPC Drift Window
    **************************************/

    // drift_size: TPC drift number of samples, should be set to correspond to the time it
    // takes electrons to drift from cathode to anode.
    uint32_t drift_size_;

    /************************************
     *  Trigger parameters
    **************************************/

    // trigger_source: What triggers the system (software, light, external)
    uint32_t trigger_source_;
    // software_trigger_rate_hz: Only used when the software trigger is selected as the source, sets the trigger rate in Hz
    uint32_t software_trigger_rate_hz_;
    // dead_time: Dead time after a trigger until another trigger is accepted. In 32us increments
    uint32_t tpc_dead_time_;
    // light_trig_prescale: There are number of prescales for various trigger sources used in previous applications e.g. uBoonNE
    // We only care about prescaling the light trigger so use a seperate config parameter for convenience.
    uint32_t light_trig_prescale_;
    // prescale: The rest of the prescales, not planning to use for pGRAMS.
    std::array<uint32_t, NUM_PRESCALES> prescale_;

    /************************************
     *  Discriminator thresholds
    **************************************/

    // channel_thresh0: SiPM discriminator threshold 0, when crossed it arms the system for discriminator 1
    std::array<uint32_t, NUM_LIGHT_CHANNELS> disc_threshold_0_;
    // channel_thresh1: SiPM discriminator threshold 1, when threshold is crossed the ROI is saved to the FIFO
    std::array<uint32_t, NUM_LIGHT_CHANNELS> disc_threshold_1_;


    /************************************
     *  Unused, set to these default values
    **************************************/

    // These are parameters used for beam and Michel triggers, both which are not used

    uint32_t dma_buffer_kB_; // just an extra one
    uint32_t beam_multiplicity_ = 100; // : 100,
    uint32_t beam_summed_adc_thresh_ = 500; // : 500,
    uint32_t michel_multiplicity_ = 100; // : 100,
    uint32_t michel_summed_adc_thresh_ = 100; // : 500,
    std::array<uint32_t, NUM_LIGHT_CHANNELS> disc_threshold_3_;
    std::array<uint32_t, NUM_LIGHT_CHANNELS> disc_threshold_4_;

    // // Implement  the serialize/deserialize
    size_t num_members_ = 20;

    auto member_tuple() {
        return std::tie(config_file_number_, summed_peak_thresh_, channel_multiplicity_,
        roi_delay_0_, roi_delay_1_, roi_precount_, roi_peak_window_,
        enable_top_, enable_middle_, enable_bottom_, num_roi_words_,
        roi_deadtime_, fifo_blocksize_, drift_size_, trigger_source_,
        software_trigger_rate_hz_,tpc_dead_time_, light_trig_prescale_,
        unbiased_light_samples_, dma_buffer_kB_);
    };

    auto member_tuple() const {
        return std::tie(config_file_number_, summed_peak_thresh_, channel_multiplicity_,
        roi_delay_0_, roi_delay_1_, roi_precount_, roi_peak_window_,
        enable_top_, enable_middle_, enable_bottom_, num_roi_words_,
        roi_deadtime_, fifo_blocksize_, drift_size_, trigger_source_,
        software_trigger_rate_hz_, tpc_dead_time_, light_trig_prescale_,
        unbiased_light_samples_, dma_buffer_kB_);
    };

public:
    TpcConfigs();

    void clear();
    void print() const;

    // MetricBase interface implementation
    std::vector<uint32_t> serialize() const override;
    std::vector<uint32_t>::const_iterator deserialize(std::vector<uint32_t>::const_iterator begin,
                                                     std::vector<uint32_t>::const_iterator end) override;

    // Helper for trigger selection
    std::string toTriggerSourceString(uint32_t code) {
        switch (code) {
            case 0: return "light";
            case 1: return "software";
            case 2: return "external";
            default: return "unknown";
        }
    }

#ifdef USE_PYTHON
    py::dict getMetricDict() override;
    void setMetricDict(py::dict &config);

    // Config setter helper functions
    template<typename T>
    void AssignScalar(T &config_param, py::dict &config, std::string config_key) {
        std::cout << "Config/type " << config_key << "/" << typeid(T).name() << std::endl;
        if (!config.contains(py::str(config_key)) ) {
            throw std::runtime_error("Missing key [" + config_key + "]");
        }
        config_param = config[py::str(config_key)].cast<T>();
    }

    template <size_t N>
    void AssignArray(std::array<uint32_t, N> &param_vec, py::dict &config, const std::string &config_key) {
        if (!config.contains(py::str(config_key))) {
            throw std::runtime_error("Missing key [" + config_key + "]");
        }

        py::object obj = config[py::str(config_key)];
        std::vector<uint32_t> tmp;

        if (py::isinstance<py::array>(obj)) {
            // Handle numpy array (forcecast ensures conversion to uint32_t)
            py::array_t<uint32_t, py::array::c_style | py::array::forcecast> arr = py::cast<py::array>(obj);
            if (arr.ndim() != 1) {
                throw std::runtime_error("Expected 1D array for " + config_key);
            }
            tmp.resize(arr.size());
            std::memcpy(tmp.data(), arr.data(), arr.size() * sizeof(uint32_t));
        } else if (py::isinstance<py::sequence>(obj)) {
            // Handle Python list/tuple
            py::sequence seq = py::reinterpret_borrow<py::sequence>(obj);
            tmp.resize(seq.size());
            for (size_t i = 0; i < seq.size(); i++) {
                tmp[i] = seq[i].cast<uint32_t>();
            }
        } else {
            throw std::runtime_error("Expected list or numpy array for " + config_key);
        }

        // Validate size
        if (tmp.size() != N) {
            throw std::runtime_error("Incorrect number of " + config_key + " thresholds! Expected/Received " +
                                     std::to_string(N) + "/" + std::to_string(tmp.size()));
        }
        // Copy into std::array
        std::copy(tmp.begin(), tmp.end(), param_vec.begin());
    }
#endif

    // ===== Getters & Setters =====

    // Scalars
    uint32_t getConfigFileNumber() const { return config_file_number_; }
    void setConfigFileNumber(uint32_t v) { config_file_number_ = v; }

    uint32_t getSummedPeakThresh() const { return summed_peak_thresh_; }
    void setSummedPeakThresh(uint32_t v) { summed_peak_thresh_ = v; }

    uint32_t getChannelMultiplicity() const { return channel_multiplicity_; }
    void setChannelMultiplicity(uint32_t v) { channel_multiplicity_ = v; }

    uint32_t getRoiDelay0() const { return roi_delay_0_; }
    void setRoiDelay0(uint32_t v) { roi_delay_0_ = v; }

    uint32_t getRoiDelay1() const { return roi_delay_1_; }
    void setRoiDelay1(uint32_t v) { roi_delay_1_ = v; }

    uint32_t getRoiPrecount() const { return roi_precount_; }
    void setRoiPrecount(uint32_t v) { roi_precount_ = v; }

    uint32_t getRoiPeakWindow() const { return roi_peak_window_; }
    void setRoiPeakWindow(uint32_t v) { roi_peak_window_ = v; }

    uint32_t getEnableTop() const { return enable_top_; }
    void setEnableTop(uint32_t v) { enable_top_ = v; }

    uint32_t getEnableMiddle() const { return enable_middle_; }
    void setEnableMiddle(uint32_t v) { enable_middle_ = v; }

    uint32_t getEnableBottom() const { return enable_bottom_; }
    void setEnableBottom(uint32_t v) { enable_bottom_ = v; }

    uint32_t getNumRoiWords() const { return num_roi_words_; }
    void setNumRoiWords(uint32_t v) { num_roi_words_ = v; }

    uint32_t getRoiDeadtime() const { return roi_deadtime_; }
    void setRoiDeadtime(uint32_t v) { roi_deadtime_ = v; }

    uint32_t getFifoBlocksize() const { return fifo_blocksize_; }
    void setFifoBlocksize(uint32_t v) { fifo_blocksize_ = v; }

    uint32_t getDriftSize() const { return drift_size_; }
    void setDriftSize(uint32_t v) { drift_size_ = v; }

    uint32_t getTriggerSource() const { return trigger_source_; }
    void setTriggerSource(uint32_t v) { trigger_source_ = v; }

    uint32_t getSoftwareTriggerRateHz() const { return software_trigger_rate_hz_; }
    void setSoftwareTriggerRateHz(uint32_t v) { software_trigger_rate_hz_ = v; }

    uint32_t getTpcDeadTime() const { return tpc_dead_time_; }
    void setTpcDeadTime(uint32_t v) { tpc_dead_time_ = v; }

    uint32_t getLightTrigPrescale() const { return light_trig_prescale_; }
    void setLightTrigPrescale(uint32_t v) { light_trig_prescale_ = v; }
   
    uint32_t getUnbiasedLightSamples() const { return unbiased_light_samples_; }   
    void setUnbiasedLightSamples(uint32_t v) { unbiased_light_samples_ = v; }

    uint32_t getDmaBufferkB() const { return dma_buffer_kB_; }   
    void setDmaBufferkB(uint32_t v) { dma_buffer_kB_ = v; }

    // prescale
    const std::array<uint32_t, NUM_PRESCALES>& getPrescale() const { return prescale_; }
    std::array<uint32_t, NUM_PRESCALES>& getPrescale() { return prescale_; }
    void setPrescale(const std::array<uint32_t, NUM_PRESCALES>& v) { prescale_ = v; }

    // disc_threshold_0
    const std::array<uint32_t, NUM_LIGHT_CHANNELS>& getDiscThreshold0() const { return disc_threshold_0_; }
    std::array<uint32_t, NUM_LIGHT_CHANNELS>& getDiscThreshold0() { return disc_threshold_0_; }
    void setDiscThreshold0(const std::array<uint32_t, NUM_LIGHT_CHANNELS>& v) { disc_threshold_0_ = v; }

    // disc_threshold_1
    const std::array<uint32_t, NUM_LIGHT_CHANNELS>& getDiscThreshold1() const { return disc_threshold_1_; }
    std::array<uint32_t, NUM_LIGHT_CHANNELS>& getDiscThreshold1() { return disc_threshold_1_; }
    void setDiscThreshold1(const std::array<uint32_t, NUM_LIGHT_CHANNELS>& v) { disc_threshold_1_ = v; }

    // disc_threshold_3
    const std::array<uint32_t, NUM_LIGHT_CHANNELS>& getDiscThreshold3() const { return disc_threshold_3_; }
    std::array<uint32_t, NUM_LIGHT_CHANNELS>& getDiscThreshold3() { return disc_threshold_3_; }
    void setDiscThreshold3(const std::array<uint32_t, NUM_LIGHT_CHANNELS>& v) { disc_threshold_3_ = v; }

    // disc_threshold_4
    const std::array<uint32_t, NUM_LIGHT_CHANNELS>& getDiscThreshold4() const { return disc_threshold_4_; }
    std::array<uint32_t, NUM_LIGHT_CHANNELS>& getDiscThreshold4() { return disc_threshold_4_; }
    void setDiscThreshold4(const std::array<uint32_t, NUM_LIGHT_CHANNELS>& v) { disc_threshold_4_ = v; }
};

#endif //TPC_CONFIGS_H

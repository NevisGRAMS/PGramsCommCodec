//
// Created by Jon Sensenig on 8/23/25.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // Needed for automatic vector conversion
#include "../include/metric_base.h"
#include "../include/histogram.h"
#include "../include/tpc_monitor.h"
#include "../include/tpc_monitor_lbw.h"
#include "../include/tpc_configs.h"
#include "../include/daq_comp_monitor.h"
#include "../include/tpc_readout_monitor.h"
#include "../include/tpc_monitor_charge_event.h"
#include "../include/tpc_monitor_light_event.h"
#include "CommunicationCodes.hh"

namespace py = pybind11;

// A trampoline class is needed for pybind11 to handle virtual functions
// that might be overridden in Python.
class PyMetricBase : public MetricBase {
public:
    // Inherit constructors
    using MetricBase::MetricBase;

    std::vector<uint32_t> serialize() const override {
        PYBIND11_OVERRIDE_PURE(
            std::vector<uint32_t>, /* Return type */
            MetricBase,           /* Parent class */
            serialize             /* Function name */
        );
    }

    // Trampoline for the pure virtual deserialize
    std::vector<uint32_t>::const_iterator deserialize(
        std::vector<uint32_t>::const_iterator begin,
        std::vector<uint32_t>::const_iterator end) override {
        PYBIND11_OVERRIDE_PURE(
            std::vector<uint32_t>::const_iterator, /* Return type */
            MetricBase,                           /* Parent class */
            deserialize,                          /* Function name */
            begin, end                            /* Arguments */
        );
    }

    // Trampoline for the pure virtual getMetricDict
    py::dict getMetricDict() override {
        PYBIND11_OVERRIDE_PURE(
            py::dict,       /* Return type */
            MetricBase,     /* Parent class */
            getMetricDict   /* Function name */
        );
    }
};

// The PYBIND11_MODULE macro creates the entry point for the Python module.
// The first argument is the module name. It MUST match the filename of the final library.
PYBIND11_MODULE(datamon, m) {
    m.doc() = "Python bindings for the data monitoring library"; // Optional module docstring

    // We don't define a constructor (.def(py::init<>())) because it's an abstract interface.
    // This just makes pybind11 aware of the type so derived classes can use it.
    py::class_<MetricBase, PyMetricBase /* trampoline */>(m, "MetricBase")
        .def(py::init<>())
        .def("deserialize", static_cast<void (MetricBase::*)(const std::vector<uint32_t>&)>(&MetricBase::deserialize), "Deserialize data from a list of integers.")
        .def("get_metric_dict", &MetricBase::getMetricDict, "Deserialize data and return a dictionary.");

    // Command enum class bindings
    py::enum_<pgrams::communication::CommunicationCodes>(m, "CommCodes")
        // Orchestrator
        .value("OrcBootAllDaq", pgrams::communication::CommunicationCodes::ORC_Boot_All_DAQ)
        .value("OrcShutdownAllDaq", pgrams::communication::CommunicationCodes::ORC_Shutdown_All_DAQ)
        .value("OrcStartComputerStatus", pgrams::communication::CommunicationCodes::ORC_Start_Computer_Status)
        .value("OrcStopComputerStatus", pgrams::communication::CommunicationCodes::ORC_Stop_Computer_Status)
        .value("OrcExecCpuRestart", pgrams::communication::CommunicationCodes::ORC_Exec_CPU_Restart)
        .value("OrcExecCpuShutdown", pgrams::communication::CommunicationCodes::ORC_Exec_CPU_Shutdown)
        .value("OrcPcieInit", pgrams::communication::CommunicationCodes::ORC_Init_PCIe_Driver)
        .value("OrcBootMonitor", pgrams::communication::CommunicationCodes::ORC_Boot_Monitor)
        .value("OrcShutdownMonitor", pgrams::communication::CommunicationCodes::ORC_Shutdown_Monitor)
        .value("OrcBootTof", pgrams::communication::CommunicationCodes::ORC_Boot_Tof_Daq)
        .value("OrcShutdownTof", pgrams::communication::CommunicationCodes::ORC_Shutdown_Tof_Daq)

        // TPC Readout
        .value("ColConfigure", pgrams::communication::CommunicationCodes::TPC_Configure)
        .value("ColStartRun", pgrams::communication::CommunicationCodes::TPC_Start_Run)
        .value("ColStopRun", pgrams::communication::CommunicationCodes::TPC_Stop_Run)
        .value("ColResetRun", pgrams::communication::CommunicationCodes::TPC_Reset_Run)
        .value("ColQueryLBData", pgrams::communication::CommunicationCodes::TPCMonitor_Query_LB_Data)
        .value("ColQueryEventData", pgrams::communication::CommunicationCodes::TPCMonitor_Query_Event_Data)
        .export_values();


    py::enum_<pgrams::communication::TelemetryCodes>(m, "TelemCodes")
        // Orchestrator
        .value("OrcHardwareStatus", pgrams::communication::TelemetryCodes::ORC_Hardware_Status)
        // TPC Readout
        .value("ColHardwareStatus", pgrams::communication::TelemetryCodes::TPC_Hardware_Status)
        .value("ColQueryHardwareStatus", pgrams::communication::TelemetryCodes::TPC_Query_Hardware_Status)
        .export_values();

    // Bind the Histogram class
    py::class_<Histogram, MetricBase>(m, "Histogram")
        .def(py::init<>()) // Bind the default constructor
        .def(py::init<uint32_t, uint32_t, uint32_t>()) // Bind the parameterized constructor
        .def("fill", &Histogram::fill, "Fill the histogram with a value")
        .def("clear", &Histogram::clear, "Clear the histogram data")
        .def("serialize", &Histogram::serialize, "Serialize the histogram to a list of ints")

        .def_property_readonly("min_value", &Histogram::getMinValue)
        .def_property_readonly("max_value", &Histogram::getMaxValue)
        .def_property_readonly("num_bins", &Histogram::getNumBins)
        .def_property_readonly("bins", &Histogram::getBins)
        .def_property_readonly("below_range_count", &Histogram::getBelowRangeCount)
        .def_property_readonly("above_range_count", &Histogram::getAboveRangeCount);

    // Bind the TpcMonitor class
    py::class_<TpcMonitor, MetricBase>(m, "TpcMonitor")
        .def(py::init<>())
        .def("clear", &TpcMonitor::clear)
        .def("serialize", &TpcMonitor::serialize)

        // Expose the histograms (e.g., as read-only properties)
        .def_property_readonly("charge_histograms", &TpcMonitor::getChargeHistograms)
        .def_property_readonly("light_histograms", &TpcMonitor::getLightHistograms);

    // Bind the LowBwTpcMonitor class
    py::class_<LowBwTpcMonitor, MetricBase>(m, "LowBwTpcMonitor")
        .def(py::init<>())
        .def("clear", &LowBwTpcMonitor::clear)
        .def("serialize", &LowBwTpcMonitor::serialize);

    // Bind the TpcMonitorChargeEvent class
    py::class_<TpcMonitorChargeEvent, MetricBase>(m, "TpcMonitorChargeEvent")
        .def(py::init<>())
        .def("clear", &TpcMonitorChargeEvent::clear)
        .def("serialize", &TpcMonitorChargeEvent::serialize);

    // Bind the TpcMonitorLightEvent class
    py::class_<TpcMonitorLightEvent, MetricBase>(m, "TpcMonitorLightEvent")
        .def(py::init<>())
        .def("clear", &TpcMonitorLightEvent::clear)
        .def("serialize", &TpcMonitorLightEvent::serialize);

    // Bind the TpcConfigs class
    py::class_<TpcConfigs, MetricBase>(m, "TpcConfig")
        .def(py::init<>())
        .def("clear", &TpcConfigs::clear)
        .def("serialize", &TpcConfigs::serialize)
        .def("to_trigger_source_string", &TpcConfigs::toTriggerSourceString)
        .def("set_config_dict", &TpcConfigs::setMetricDict)
        .def("print", &TpcConfigs::print);

    // Bind the TPC Readout Monitor class
    py::class_<TpcReadoutMonitor, MetricBase>(m, "TpcReadoutMonitor")
        .def(py::init<>())
        .def("clear", &TpcReadoutMonitor::clear)
        .def("serialize", &TpcReadoutMonitor::serialize)
        .def("print", &TpcReadoutMonitor::print);


    // Bind the DaqCompMonitor class
    py::class_<DaqCompMonitor, MetricBase>(m, "DaqCompMonitor")
        .def(py::init<>())
        .def("clear", &DaqCompMonitor::clear)
        .def("serialize", &DaqCompMonitor::serialize)

        .def_property_readonly("daq_bit_word", &DaqCompMonitor::getFullDaqBitWord)
        .def_property_readonly("tpc_disk0", &DaqCompMonitor::getTpcDisk0)
        .def_property_readonly("tof_disk0", &DaqCompMonitor::getTofDisk0)
        .def_property_readonly("tpc_disk1", &DaqCompMonitor::getTpcDisk1)
        .def_property_readonly("tof_disk1", &DaqCompMonitor::getTofDisk1)
        .def_property_readonly("sys_disk", &DaqCompMonitor::getSysDisk)
        .def_property_readonly("cpu_usage", &DaqCompMonitor::getCpuUsage)
        .def_property_readonly("memory_usage", &DaqCompMonitor::getMemoryUsage)
        .def_property_readonly("disk_temp", &DaqCompMonitor::getDiskTemp)
        .def_property_readonly("cpu_temp", &DaqCompMonitor::getCpuTemp);
}


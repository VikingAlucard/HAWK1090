#include "ADSBdecoder_MR_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

ADSBdecoder_MR_base::ADSBdecoder_MR_base(const char *uuid, const char *label) :
    Component(uuid, label),
    ThreadedComponent()
{
    setThreadName(label);

    loadProperties();

    input = new bulkio::InOctetPort("input");
    input->setLogger(this->_baseLog->getChildLogger("input", "ports"));
    addPort("input", input);
    rawMessages = new bulkio::OutOctetPort("rawMessages");
    rawMessages->setLogger(this->_baseLog->getChildLogger("rawMessages", "ports"));
    addPort("rawMessages", rawMessages);
    processedTargets = new bulkio::OutOctetPort("processedTargets");
    processedTargets->setLogger(this->_baseLog->getChildLogger("processedTargets", "ports"));
    addPort("processedTargets", processedTargets);
}

ADSBdecoder_MR_base::~ADSBdecoder_MR_base()
{
    input->_remove_ref();
    input = 0;
    rawMessages->_remove_ref();
    rawMessages = 0;
    processedTargets->_remove_ref();
    processedTargets = 0;
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void ADSBdecoder_MR_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    Component::start();
    ThreadedComponent::startThread();
}

void ADSBdecoder_MR_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    Component::stop();
    if (!ThreadedComponent::stopThread()) {
        throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
    }
}

void ADSBdecoder_MR_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    Component::releaseObject();
}

void ADSBdecoder_MR_base::loadProperties()
{
    addProperty(DebugMode,
                0,
                "DebugMode",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(InteractiveMode,
                1,
                "InteractiveMode",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(StatsMode,
                0,
                "StatsMode",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(userLat,
                39,
                "userLat",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(userLong,
                80,
                "userLong",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(NetMode,
                1,
                "NetMode",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(NetOnlyMode,
                "0",
                "NetOnlyMode",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(ModeAC,
                1,
                "ModeAC",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(AggressiveMode,
                1,
                "AggressiveMode",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(MessageCount,
                "MessageCount",
                "",
                "readonly",
                "",
                "external",
                "property");

    addProperty(PreambleCount,
                0LL,
                "PreambleCount",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(outputTracks,
                false,
                "outputTracks",
                "outputTracks",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(outputMessages,
                false,
                "outputMessages",
                "outputMessages",
                "readwrite",
                "",
                "external",
                "property");

}



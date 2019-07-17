#ifndef ADSBDECODER_MR_BASE_IMPL_BASE_H
#define ADSBDECODER_MR_BASE_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Component.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>

class ADSBdecoder_MR_base : public Component, protected ThreadedComponent
{
    public:
        ADSBdecoder_MR_base(const char *uuid, const char *label);
        ~ADSBdecoder_MR_base();

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void loadProperties();

    protected:
        // Member variables exposed as properties
        /// Property: DebugMode
        short DebugMode;
        /// Property: InteractiveMode
        short InteractiveMode;
        /// Property: StatsMode
        short StatsMode;
        /// Property: userLat
        double userLat;
        /// Property: userLong
        double userLong;
        /// Property: NetMode
        short NetMode;
        /// Property: NetOnlyMode
        std::string NetOnlyMode;
        /// Property: ModeAC
        short ModeAC;
        /// Property: AggressiveMode
        short AggressiveMode;
        /// Property: MessageCount
        CORBA::LongLong MessageCount;
        /// Property: PreambleCount
        CORBA::ULongLong PreambleCount;
        /// Property: outputTracks
        bool outputTracks;
        /// Property: outputMessages
        bool outputMessages;

        // Ports
        /// Port: input
        bulkio::InOctetPort *input;
        /// Port: rawMessages
        bulkio::OutOctetPort *rawMessages;
        /// Port: processedTargets
        bulkio::OutOctetPort *processedTargets;

    private:
};
#endif // ADSBDECODER_MR_BASE_IMPL_BASE_H

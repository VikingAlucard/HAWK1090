/*
"This software was developed at SPAWAR Systems Center Atlantic by employees of the Federal Government in the course of their official duties. Pursuant to title 17 Section 105 of the United States Code this software is not subject to copyright protection and is in the public domain. The Government assumes no responsibility whatsoever for its use by other parties, and the software is provided "AS IS" without warranty or guarantee of any kind, express or implied about its quality, reliability, or any other characteristic. In no event shall the Government be liable for any claim, damages or other liability, whether in an action of contract, tort or other dealings in the software.   The Government has no obligation hereunder to provide maintenance, support, updates, enhancements, or modifications.  We would appreciate acknowledgement if the software is used. This software can be redistributed and/or modified freely provided that any derivative works bear some notice that they are derived from it, and any modified versions bear some notice that they have been modified." 
*/

/**************************************************************************


 This is the component code. This file contains the child class where
 custom functionality can be added to the component. Custom
 functionality to the base class can be extended here. Access to
 the ports can also be done from this class

 **************************************************************************/

#include "ADSBdecoder_MR.h"
#include <iostream>

using namespace std;

PREPARE_LOGGING(ADSBdecoder_MR_i)

// Declare a global instance of the Modes1 struct called "Modes"
Modes1 Modes;

ADSBdecoder_MR_i::ADSBdecoder_MR_i(const char *uuid, const char *label) :
ADSBdecoder_MR_base(uuid, label) {

	buf_size = MODES_ASYNC_BUF_SAMPLES;

	modesInitConfig();

	Modes.debug = DebugMode;
	Modes.stats = StatsMode;
	Modes.interactive = InteractiveMode;
	Modes.nfix_crc = MODES_MAX_BITERRORS;
	Modes.fUserLat = userLat;
	Modes.fUserLon = userLong;
	Modes.mode_ac = ModeAC;

	modesInit();

	Modes.net = NetMode;
	if (Modes.net) {
		modesInitNet();
	}

	addPropertyChangeListener("InteractiveMode", this, &ADSBdecoder_MR_i::setInteractiveMode);
}

ADSBdecoder_MR_i::~ADSBdecoder_MR_i() {
	// Displays statistics upon teardown of waveform
	if (Modes.stats) {
		display_stats();
	}
}

void ADSBdecoder_MR_i::setInteractiveMode(const short *oldValue, const short *newValue) {
	InteractiveMode = *newValue;
	Modes.interactive = InteractiveMode;
}

/*
 * This function goes through the received IQ samples and outputs to the magnitude vector in the
 * Modes struct. It provides a magnitude calculation which is used to demodulate the ADSB signal.
 * Implemented as a look up table.
 */
 void ADSBdecoder_MR_i::computeMagnitudeVector(std::vector<uint16_t> p, uint32_t in_buffer_length) {

	uint16_t *m = &Modes.magnitude[MODES_PREAMBLE_SAMPLES + MODES_LONG_MSG_SAMPLES];
 	uint32_t j;

 	// This ensures that the last possible preamble and message in the buffer is not lost
 	memcpy(Modes.magnitude, &(Modes.magnitude[in_buffer_length]), MODES_PREAMBLE_SIZE + MODES_LONG_MSG_SIZE);

    // Assign a magnitude value to output buffer.
 	for (j = 0; j < in_buffer_length; j++) {
 		*m++ = Modes.maglut[p[j]];
 	}
 }

/***********************************************************************************************

 Basic functionality:

 The service function is called by the serviceThread object (of type ProcessThread).
 This call happens immediately after the previous call if the return value for
 the previous call was NORMAL.
 If the return value for the previous call was NOOP, then the serviceThread waits
 an amount of time defined in the serviceThread's constructor.

 SRI:
 To create a StreamSRI object, use the following code:
 std::string stream_id = "testStream";
 BULKIO::StreamSRI sri = bulkio::sri::create(stream_id);

 Time:
 To create a PrecisionUTCTime object, use the following code:
 BULKIO::PrecisionUTCTime tstamp = bulkio::time::utils::now();


 Ports:

 Data is passed to the serviceFunction through the getPacket call (BULKIO only).
 The dataTransfer class is a port-specific class, so each port implementing the
 BULKIO interface will have its own type-specific dataTransfer.

 The argument to the getPacket function is a floating point number that specifies
 the time to wait in seconds. A zero value is non-blocking. A negative value
 is blocking.  Constants have been defined for these values, bulkio::Const::BLOCKING and
 bulkio::Const::NON_BLOCKING.

 Each received dataTransfer is owned by serviceFunction and *MUST* be
 explicitly deallocated.

 To send data using a BULKIO interface, a convenience interface has been added
 that takes a std::vector as the data input

 NOTE: If you have a BULKIO dataSDDS or dataVITA49  port, you must manually call
 "port->updateStats()" to update the port statistics when appropriate.

 Example:
 // this example assumes that the component has two ports:
 //  A provides (input) port of type bulkio::InShortPort called short_in
 //  A uses (output) port of type bulkio::OutFloatPort called float_out
 // The mapping between the port and the class is found
 // in the component base class header file

 bulkio::InShortPort::dataTransfer *tmp = short_in->getPacket(bulkio::Const::BLOCKING);
 if (not tmp) { // No data is available
 return NOOP;
 }

 std::vector<float> outputData;
 outputData.resize(tmp->dataBuffer.size());
 for (unsigned int i=0; i<tmp->dataBuffer.size(); i++) {
 outputData[i] = (float)tmp->dataBuffer[i];
 }

 // NOTE: You must make at least one valid pushSRI call
 if (tmp->sriChanged) {
 float_out->pushSRI(tmp->SRI);
 }
 float_out->pushPacket(outputData, tmp->T, tmp->EOS, tmp->streamID);

 delete tmp; // IMPORTANT: MUST RELEASE THE RECEIVED DATA BLOCK
 return NORMAL;

 If working with complex data (i.e., the "mode" on the SRI is set to
 true), the std::vector passed from/to BulkIO can be typecast to/from
 std::vector< std::complex<dataType> >.  For example, for short data:

 bulkio::InShortPort::dataTransfer *tmp = myInput->getPacket(bulkio::Const::BLOCKING);
 std::vector<std::complex<short> >* intermediate = (std::vector<std::complex<short> >*) &(tmp->dataBuffer);
 // do work here
 std::vector<short>* output = (std::vector<short>*) intermediate;
 myOutput->pushPacket(*output, tmp->T, tmp->EOS, tmp->streamID);

 Interactions with non-BULKIO ports are left up to the component developer's discretion

 Properties:

 Properties are accessed directly as member variables. For example, if the
 property name is "baudRate", it may be accessed within member functions as
 "baudRate". Unnamed properties are given the property id as its name.
 Property types are mapped to the nearest C++ type, (e.g. "string" becomes
 "std::string"). All generated properties are declared in the base class
 (ADSBdecoder_MR_base).

 Simple sequence properties are mapped to "std::vector" of the simple type.
 Struct properties, if used, are mapped to C++ structs defined in the
 generated file "struct_props.h". Field names are taken from the name in
 the properties file; if no name is given, a generated name of the form
 "field_n" is used, where "n" is the ordinal number of the field.

 Example:
 // This example makes use of the following Properties:
 //  - A float value called scaleValue
 //  - A boolean called scaleInput

 if (scaleInput) {
 dataOut[i] = dataIn[i] * scaleValue;
 } else {
 dataOut[i] = dataIn[i];
 }

 Callback methods can be associated with a property so that the methods are
 called each time the property value changes.  This is done by calling
 addPropertyChangeListener(<property name>, this, &ADSBdecoder_MR_i::<callback method>)
 in the constructor.

 Callback methods should take two arguments, both const pointers to the value
 type (e.g., "const float *"), and return void.

 Example:
 // This example makes use of the following Properties:
 //  - A float value called scaleValue

 //Add to ADSBdecoder_MR.cpp
 ADSBdecoder_MR_i::ADSBdecoder_MR_i(const char *uuid, const char *label) :
 ADSBdecoder_MR_base(uuid, label)
 {
 addPropertyChangeListener("scaleValue", this, &ADSBdecoder_MR_i::scaleChanged);
 }

 void ADSBdecoder_MR_i::scaleChanged(const float *oldValue, const float *newValue)
 {
 std::cout << "scaleValue changed from" << *oldValue << " to " << *newValue
 << std::endl;
 }

 //Add to ADSBdecoder_MR.h
 void scaleChanged(const float* oldValue, const float* newValue);


 ************************************************************************************************/

int ADSBdecoder_MR_i::serviceFunction() {

	LOG_DEBUG(ADSBdecoder_MR_i, "serviceFunction() example log message");

	string stream_id = "testStream";
	string stream_out_id = "targets";
	BULKIO::StreamSRI sri = bulkio::sri::create(stream_id);
	BULKIO::PrecisionUTCTime tstamp = bulkio::time::utils::now();

	bulkio::InOctetPort::dataTransfer *tmp = input->getPacket(bulkio::Const::BLOCKING);

	if (not tmp) { // No data is available
		return NOOP;
	}

	uint32_t buffer_length = tmp->dataBuffer.size() / 2;

	// If the input buffer size does not equal the default value, resize Modes.magnitude accordingly to avoid crash
	if (buf_size != buffer_length) {
		buf_size = buffer_length;
		free(Modes.magnitude);
		Modes.magnitude = (uint16_t *) malloc((buf_size * 2) + MODES_PREAMBLE_SIZE + MODES_LONG_MSG_SIZE);
		memset(Modes.magnitude, 0, (buf_size * 2) + MODES_PREAMBLE_SIZE + MODES_LONG_MSG_SIZE);
	}

	std::vector<uint16_t> Data(tmp->dataBuffer.size() / 2);

	// Combines two char's (interleaved I/Q samples) into one uint16_t
	for (unsigned int i = 0; i < tmp->dataBuffer.size(); i += 2)
	{
		Data[i / 2] = (tmp->dataBuffer[i + 1] << 8) | tmp->dataBuffer[i];
	}

	// Compute magnitude of input signal
	computeMagnitudeVector(Data, buffer_length);

	// Detect Mode S and optionally Mode A/C messages in the input buffer, then decode them
	detectModeS(Modes.magnitude, buffer_length);

	// Takes care of background processes
	backgroundTasks();
	// Get JSON data and push
	if(outputTracks){
		std::vector<std::string> jsonArr = getJSONData();
		for (string str : jsonArr){
			std::cout << "Pushing JSON:" << str << std::endl;
			std::vector<unsigned char> data(str.begin(),str.end());
			processedTargets->pushPacket(data,tstamp,false,stream_out_id);
		}
	}

	// Compute statistics used for unit testing
	MessageCount = Modes.stat_goodcrc + Modes.stat_ph_goodcrc + Modes.stat_fixed + Modes.stat_ph_fixed;
	PreambleCount = Modes.stat_valid_preamble;

	delete tmp;

	return NORMAL;

}


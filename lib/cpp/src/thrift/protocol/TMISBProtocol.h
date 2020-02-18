/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _THRIFT_PROTOCOL_TMISBPROTOCOL_H_
#define _THRIFT_PROTOCOL_TMISBPROTOCOL_H_ 1

#include <thrift/protocol/TProtocol.h>
#include <thrift/protocol/TVirtualProtocol.h>
#include "St060115Tag.h"

#include <memory>
#include <unordered_map>

namespace apache {
namespace thrift {
namespace protocol {

/**
 * The default binary protocol for thrift. Writes all data in a very basic
 * binary format, essentially just spitting out the raw bytes.
 *
 */
template <class Transport_, class ByteOrder_ = TNetworkBigEndian>
class TMISBProtocolT : public TVirtualProtocol<TMISBProtocolT<Transport_, ByteOrder_> > {
public:
  static const int32_t VERSION_MASK = ((int32_t)0xffff0000);
  static const int32_t VERSION_1 = ((int32_t)0x80010000);
  // VERSION_2 (0x80020000) was taken by TDenseProtocol (which has since been removed)

  std::unordered_map<int16_t,TType> fieldIdToTTypeMap;
  TType fieldIdToTType( int16_t fieldId ) {
    auto it = fieldIdToTTypeMap.find( fieldId );
    if ( fieldIdToTTypeMap.end() == it ) {
      throw TProtocolException(TProtocolException::INVALID_DATA);
    }
    return it->second;
  }

  TMISBProtocolT(std::shared_ptr<Transport_> trans)
    : TVirtualProtocol<TMISBProtocolT<Transport_, ByteOrder_> >(trans),
      trans_(trans.get()),
      string_limit_(0),
      container_limit_(0),
      strict_read_(false),
      strict_write_(true) {
	  initializeFieldIdToTTypeMap();
  }

  TMISBProtocolT(std::shared_ptr<Transport_> trans,
                   int32_t string_limit,
                   int32_t container_limit,
                   bool strict_read,
                   bool strict_write)
    : TVirtualProtocol<TMISBProtocolT<Transport_, ByteOrder_> >(trans),
      trans_(trans.get()),
      string_limit_(string_limit),
      container_limit_(container_limit),
      strict_read_(strict_read),
      strict_write_(strict_write) {
	  initializeFieldIdToTTypeMap();
  }

  void setStringSizeLimit(int32_t string_limit) { string_limit_ = string_limit; }

  void setContainerSizeLimit(int32_t container_limit) { container_limit_ = container_limit; }

  void setStrict(bool strict_read, bool strict_write) {
    strict_read_ = strict_read;
    strict_write_ = strict_write;
  }

  /**
   * Writing functions.
   */

  /*ol*/ uint32_t writeMessageBegin(const std::string& name,
                                    const TMessageType messageType,
                                    const int32_t seqid);

  /*ol*/ uint32_t writeMessageEnd();

  inline uint32_t writeStructBegin(const char* name);

  inline uint32_t writeStructEnd();

  inline uint32_t writeFieldBegin(const char* name, const TType fieldType, const int16_t fieldId);

  inline uint32_t writeFieldEnd();

  inline uint32_t writeFieldStop();

  inline uint32_t writeMapBegin(const TType keyType, const TType valType, const uint32_t size);

  inline uint32_t writeMapEnd();

  inline uint32_t writeListBegin(const TType elemType, const uint32_t size);

  inline uint32_t writeListEnd();

  inline uint32_t writeSetBegin(const TType elemType, const uint32_t size);

  inline uint32_t writeSetEnd();

  inline uint32_t writeBool(const bool value);

  inline uint32_t writeByte(const int8_t byte);

  inline uint32_t writeI16(const int16_t i16);

  inline uint32_t writeI32(const int32_t i32);

  inline uint32_t writeI64(const int64_t i64);

  inline uint32_t writeDouble(const double dub);

  template <typename StrType>
  inline uint32_t writeString(const StrType& str);

  inline uint32_t writeBinary(const std::string& str);

  /**
   * Reading functions
   */

  /*ol*/ uint32_t readMessageBegin(std::string& name, TMessageType& messageType, int32_t& seqid);

  /*ol*/ uint32_t readMessageEnd();

  inline uint32_t readStructBegin(std::string& name);

  inline uint32_t readStructEnd();

  inline uint32_t readFieldBegin(std::string& name, TType& fieldType, int16_t& fieldId);

  inline uint32_t readFieldEnd();

  inline uint32_t readMapBegin(TType& keyType, TType& valType, uint32_t& size);

  inline uint32_t readMapEnd();

  inline uint32_t readListBegin(TType& elemType, uint32_t& size);

  inline uint32_t readListEnd();

  inline uint32_t readSetBegin(TType& elemType, uint32_t& size);

  inline uint32_t readSetEnd();

  inline uint32_t readBool(bool& value);
  // Provide the default readBool() implementation for std::vector<bool>
  using TVirtualProtocol<TMISBProtocolT<Transport_, ByteOrder_> >::readBool;

  inline uint32_t readByte(int8_t& byte);

  inline uint32_t readI16(int16_t& i16);

  inline uint32_t readI32(int32_t& i32);

  inline uint32_t readI64(int64_t& i64);

  inline uint32_t readDouble(double& dub);

  template <typename StrType>
  inline uint32_t readString(StrType& str);

  inline uint32_t readBinary(std::string& str);

protected:
  template <typename StrType>
  uint32_t readStringBody(StrType& str, int32_t sz);
  void initializeFieldIdToTTypeMap() {
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CHECKSUM ] = TType::T_I16;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TIMESTAMP ] = TType::T_I64;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::MISSION_ID ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_TAIL_NUMBER ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_HEADING_ANGLE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_PITCH_ANGLE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_ROLL_ANGLE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_TRUE_AIRSPEED ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_INDICATED_AIRSPEED ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_DESIGNATION ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::IMAGE_SENSOR_SOURCE ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::IMAGE_COORDINATE_SYSTEM ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_LATTITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_LONGITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_TRUE_ALTITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_HORIZONTAL_FIELD_OF_VIEW ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_VERTICAL_FIELD_OF_VIEW ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_RELATIVE_AZIMUTH_ANGLE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_RELATIVE_ELEVATION_ANGLE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_RELATIVE_ROLL_ANGLE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SLANT_RANGE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TARGET_WIDTH ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::FRAME_CENTER_LATITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::FRAME_CENTER_LONGITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::FRAME_CENTER_ELEVATION ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OFFSET_CORNER_LATITUDE_POINT1 ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OFFSET_CORNER_LONGITUDE_POINT1 ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OFFSET_CORNER_LATITUDE_POINT2 ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OFFSET_CORNER_LONGITUDE_POINT2 ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OFFSET_CORNER_LATITUDE_POINT3 ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OFFSET_CORNER_LONGITUDE_POINT3 ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OFFSET_CORNER_LATITUDE_POINT4 ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OFFSET_CORNER_LONGITUDE_POINT4 ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ICING_DETECTED ] = TType::T_BOOL;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::WIND_DIRECTION ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::WIND_SPEED ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::STATIC_PRESSURE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::DENSITY_ALTITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OUTSIDE_AIR_TEMPERATURE ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TARGET_LOCATION_LATITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TARGET_LOCATION_LONGITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TARGET_LOCATION_ELEVATION ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TARGET_TRACK_GATE_WIDTH ] = TType::T_I16;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TARGET_TRACK_GATE_HEIGHT ] = TType::T_I16;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TARGET_ERROR_ESTIMATE_CE90 ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TARGET_ERROR_ESTIMATE_LE90 ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::GENERIC_FLAG_DATA ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SECURITY_LOCAL_SET ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::DIFFERENTIAL_PRESSURE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_ANGLE_OF_ATTACK ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_VERTICAL_SPEED ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_SIDESLIP_ANGLE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::AIRFIELD_BAROMETRIC_PRESSURE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::AIRFIELD_ELEVATION ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::RELATIVE_HUMIDITY ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_GROUND_SPEED ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::GROUND_RANGE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_FUEL_REMAINING ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_CALL_SIGN ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::WEAPON_LOAD ] = TType::T_I16;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::WEAPON_FIRED ] = TType::T_I16;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::LASER_PRF_CODE ] = TType::T_I16;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_FIELD_OF_VIEW_NAME ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_MAGNETIC_HEADING ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::UAS_DATALINK_LS_VERSION_NUMBER ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::DEPRECATED ] = TType::T_VOID;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ALTERNATE_PLATFORM_LATITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ALTERNATE_PLATFORM_LONGITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ALTERNATE_PLATFORM_ALTITUDE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ALTERNATE_PLATFORM_NAME ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ALTERNATE_PLATFORM_HEADING ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::EVENT_START_TIME_UTC ] = TType::T_I64;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::RVT_LOCAL_SET ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::VMTI_LOCAL_SET ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_ELLIPSOID_HEIGHT ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OPERATIONAL_MODE ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::FRAME_CENTER_HEIGHT_ABOVE_ELLIPSOID ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_NORTH_VELOCITY ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_EAST_VELOCITY ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::IMAGE_HORIZON_PIXEL_PACK ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CORNER_LATITUDE_POINT1_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CORNER_LONGITUDE_POINT1_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CORNER_LATITUDE_POINT2_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CORNER_LONGITUDE_POINT2_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CORNER_LATITUDE_POINT3_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CORNER_LONGITUDE_POINT3_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CORNER_LATITUDE_POINT4_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CORNER_LONGITUDE_POINT4_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_PITCH_ANGLE_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_ROLL_ANGLE_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_ANGLE_OF_ATTACK_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_SIDESLIP_ANGLE_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::MIIS_CORE_IDENTIFIER ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SAR_MOTION_IMAGERY_LOCAL_SET ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TARGET_WIDTH_EXTENDED ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::RANGE_IMAGE_LOCAL_SET ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::GEO_REGISTRATION_LOCAL_SET ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::COMPOSITE_IMAGING_LOCAL_SET ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SEGMENT_LOCAL_SET ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::AMEND_LOCAL_SET ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SDCC_FLP ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::DENSITY_ALTITUDE_EXTENDED ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_ELLIPSOID_HEIGHT_EXTENDED ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT_EXTENDED ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::STREAM_DESIGNATOR ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::OPERATIONAL_BASE ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::BROADCAST_SOURCE ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::RANGE_TO_RECOVERY_LOCATION ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TIME_AIRBORNE ] = TType::T_I32;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PROPULSION_UNIT_SPEED ] = TType::T_I32;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_COURSE_ANGLE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ALTITUDE_AGL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::RADAR_ALTIMETER ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CONTROL_COMMAND ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CONTROL_COMMAND_VERIFICATION ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_AZIMUTH_RATE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_ELEVATION_RATE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_ROLL_RATE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ON_BOARD_MI_STORAGE_PERCENT_FULL ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ACTIVE_WAVELENGTH_LIST ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::COUNTRY_CODES ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::NUMBER_OF_NAVSATS_IN_VIEW ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::POSITIONING_METHOD_SOURCE ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PLATFORM_STATUS ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_CONTROL_MODE ] = TType::T_I08;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::SENSOR_FRAME_RATE_PACK ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::WAVELENGTHS_LIST ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TARGET_ID ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::AIRBASE_LOCATIONS ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TAKE_OFF_TIME ] = TType::T_I64;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::TRANSMISSION_FREQUENCY ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ON_BOARD_MI_STORAGE_CAPACITY ] = TType::T_I32;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ZOOM_PERCENTAGE ] = TType::T_DOUBLE;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::COMMUNICATIONS_METHOD ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::LEAP_SECONDS ] = TType::T_I32;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::CORRECTION_OFFSET ] = TType::T_I64;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::PAYLOAD_LIST ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::ACTIVE_PAYLOADS ] = TType::T_STRING;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::WEAPON_STORES ] = TType::T_STRUCT;
	fieldIdToTTypeMap[ ::org::misb::St060115Tag::WAYPOINT_LIST ] = TType::T_STRUCT;
  }

  Transport_* trans_;

  int32_t string_limit_;
  int32_t container_limit_;

  // Enforce presence of version identifier
  bool strict_read_;
  bool strict_write_;
};

typedef TMISBProtocolT<TTransport> TMISBProtocol;
typedef TMISBProtocolT<TTransport, TNetworkLittleEndian> TLEMISBProtocol;

/**
 * Constructs binary protocol handlers
 */
template <class Transport_, class ByteOrder_ = TNetworkBigEndian>
class TMISBProtocolFactoryT : public TProtocolFactory {
public:
  TMISBProtocolFactoryT()
    : string_limit_(0), container_limit_(0), strict_read_(false), strict_write_(true) {}

  TMISBProtocolFactoryT(int32_t string_limit,
                          int32_t container_limit,
                          bool strict_read,
                          bool strict_write)
    : string_limit_(string_limit),
      container_limit_(container_limit),
      strict_read_(strict_read),
      strict_write_(strict_write) {}

  ~TMISBProtocolFactoryT() override = default;

  void setStringSizeLimit(int32_t string_limit) { string_limit_ = string_limit; }

  void setContainerSizeLimit(int32_t container_limit) { container_limit_ = container_limit; }

  void setStrict(bool strict_read, bool strict_write) {
    strict_read_ = strict_read;
    strict_write_ = strict_write;
  }

  std::shared_ptr<TProtocol> getProtocol(std::shared_ptr<TTransport> trans) override {
    std::shared_ptr<Transport_> specific_trans = std::dynamic_pointer_cast<Transport_>(trans);
    TProtocol* prot;
    if (specific_trans) {
      prot = new TMISBProtocolT<Transport_, ByteOrder_>(specific_trans,
                                                          string_limit_,
                                                          container_limit_,
                                                          strict_read_,
                                                          strict_write_);
    } else {
      prot = new TMISBProtocolT<TTransport, ByteOrder_>(trans,
                                                          string_limit_,
                                                          container_limit_,
                                                          strict_read_,
                                                          strict_write_);
    }

    return std::shared_ptr<TProtocol>(prot);
  }

private:
  int32_t string_limit_;
  int32_t container_limit_;
  bool strict_read_;
  bool strict_write_;
};

typedef TMISBProtocolFactoryT<TTransport> TMISBProtocolFactory;
typedef TMISBProtocolFactoryT<TTransport, TNetworkLittleEndian> TLEMISBProtocolFactory;
}
}
} // apache::thrift::protocol

#include "TMISBProtocol.tcc"

#endif // #ifndef _THRIFT_PROTOCOL_TMISBPROTOCOL_H_

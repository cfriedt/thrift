#include <unordered_map>

#include "St0601Getter.h"

using namespace std;
using namespace ::org::misb;

void St0601Getter::getTags(const unordered_set<St0601Tag> & tags, UasDataLinkLocalSet & msg /* out */ ) const {
    for( auto & t: tags ) {
        getters.at(t)(msg);
    }
}

static void getChecksum(UasDataLinkLocalSet & msg) {
    msg.__set_checksum( 0xabcd );
}
static void getTimestamp(UasDataLinkLocalSet & msg) {
    msg.__set_precisionTimeStamp( 0x0123456789abcdef );
}
static void getMissionId(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformTailNumber(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformHeadingAngle(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformPitchAngle(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformRollAngle(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformTrueAirspeed(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformIndicatedAirspeed(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformDesignation(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getImageSensorSource(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getImageCoordinateSystem(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorLattitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorLongitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorTrueAltitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorHorizontalFieldOfView(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorVerticalFieldOfView(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorRelativeAzimuthAngle(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorRelativeElevationAngle(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorRelativeRollAngle(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSlantRange(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTargetWidth(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getFrameCenterLatitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getFrameCenterLongitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getFrameCenterElevation(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOffsetCornerLatitudePoint1(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOffsetCornerLongitudePoint1(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOffsetCornerLatitudePoint2(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOffsetCornerLongitudePoint2(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOffsetCornerLatitudePoint3(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOffsetCornerLongitudePoint3(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOffsetCornerLatitudePoint4(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOffsetCornerLongitudePoint4(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getIcingDetected(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getWindDirection(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getWindSpeed(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getStaticPressure(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getDensityAltitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOutsideAirTemperature(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTargetLocationLatitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTargetLocationLongitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTargetLocationElevation(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTargetTrackGateWidth(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTargetTrackGateHeight(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTargetErrorEstimateCe90(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTargetErrorEstimateLe90(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getGenericFlagData(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSecurityLocalSet(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getDifferentialPressure(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformAngleOfAttack(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformVerticalSpeed(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformSideslipAngle(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAirfieldBarometricPressure(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAirfieldElevation(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getRelativeHumidity(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformGroundSpeed(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getGroundRange(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformFuelRemaining(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformCallSign(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getWeaponLoad(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getWeaponFired(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getLaserPrfCode(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorFieldOfViewName(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformMagneticHeading(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getUasDatalinkLsVersionNumber(UasDataLinkLocalSet & msg) {
    msg.__set_uasDatalinkLsVersionNumber(15);
}
static void getDeprecated(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAlternatePlatformLatitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAlternatePlatformLongitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAlternatePlatformAltitude(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAlternatePlatformName(UasDataLinkLocalSet & msg) {
    msg.__set_alternatePlatformName( "Thrift!" );
}
static void getAlternatePlatformHeading(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getEventStartTimeUtc(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getRvtLocalSet(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getVmtiLocalSet(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorEllipsoidHeight(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAlternatePlatformEllipsoidHeight(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOperationalMode(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getFrameCenterHeightAboveEllipsoid(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorNorthVelocity(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorEastVelocity(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getImageHorizonPixelPack(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCornerLatitudePoint1Full(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCornerLongitudePoint1Full(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCornerLatitudePoint2Full(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCornerLongitudePoint2Full(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCornerLatitudePoint3Full(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCornerLongitudePoint3Full(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCornerLatitudePoint4Full(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCornerLongitudePoint4Full(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformPitchAngleFull(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformRollAngleFull(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformAngleOfAttackFull(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformSideslipAngleFull(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getMiisCoreIdentifier(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSarMotionImageryLocalSet(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTargetWidthExtended(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getRangeImageLocalSet(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getGeoRegistrationLocalSet(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCompositeImagingLocalSet(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSegmentLocalSet(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAmendLocalSet(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSdccFlp(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getDensityAltitudeExtended(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorEllipsoidHeightExtended(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAlternatePlatformEllipsoidHeightExtended(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getStreamDesignator(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOperationalBase(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getBroadcastSource(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getRangeToRecoveryLocation(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTimeAirborne(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPropulsionUnitSpeed(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformCourseAngle(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAltitudeAgl(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getRadarAltimeter(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getControlCommand(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getControlCommandVerification(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorAzimuthRate(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorElevationRate(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorRollRate(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOnBoardMiStoragePercentFull(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getActiveWavelengthList(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCountryCodes(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getNumberOfNavsatsInView(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPositioningMethodSource(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPlatformStatus(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorControlMode(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getSensorFrameRatePack(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getWavelengthsList(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTargetId(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getAirbaseLocations(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTakeOffTime(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getTransmissionFrequency(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getOnBoardMiStorageCapacity(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getZoomPercentage(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCommunicationsMethod(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getLeapSeconds(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getCorrectionOffset(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getPayloadList(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getActivePayloads(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getWeaponStores(UasDataLinkLocalSet & msg) {
    (void) msg;
}
static void getWaypointList(UasDataLinkLocalSet & msg) {
    (void) msg;
}

static const unordered_map<St0601Tag,St0601Getter::getter> defaultGetters = {
    {St0601Tag::ST_0601_CHECKSUM, getChecksum},
    {St0601Tag::ST_0601_TIMESTAMP, getTimestamp},
    {St0601Tag::ST_0601_MISSION_ID, getMissionId},
    {St0601Tag::ST_0601_PLATFORM_TAIL_NUMBER, getPlatformTailNumber},
    {St0601Tag::ST_0601_PLATFORM_HEADING_ANGLE, getPlatformHeadingAngle},
    {St0601Tag::ST_0601_PLATFORM_PITCH_ANGLE, getPlatformPitchAngle},
    {St0601Tag::ST_0601_PLATFORM_ROLL_ANGLE, getPlatformRollAngle},
    {St0601Tag::ST_0601_PLATFORM_TRUE_AIRSPEED, getPlatformTrueAirspeed},
    {St0601Tag::ST_0601_PLATFORM_INDICATED_AIRSPEED, getPlatformIndicatedAirspeed},
    {St0601Tag::ST_0601_PLATFORM_DESIGNATION, getPlatformDesignation},
    {St0601Tag::ST_0601_IMAGE_SENSOR_SOURCE, getImageSensorSource},
    {St0601Tag::ST_0601_IMAGE_COORDINATE_SYSTEM, getImageCoordinateSystem},
    {St0601Tag::ST_0601_SENSOR_LATTITUDE, getSensorLattitude},
    {St0601Tag::ST_0601_SENSOR_LONGITUDE, getSensorLongitude},
    {St0601Tag::ST_0601_SENSOR_TRUE_ALTITUDE, getSensorTrueAltitude},
    {St0601Tag::ST_0601_SENSOR_HORIZONTAL_FIELD_OF_VIEW, getSensorHorizontalFieldOfView},
    {St0601Tag::ST_0601_SENSOR_VERTICAL_FIELD_OF_VIEW, getSensorVerticalFieldOfView},
    {St0601Tag::ST_0601_SENSOR_RELATIVE_AZIMUTH_ANGLE, getSensorRelativeAzimuthAngle},
    {St0601Tag::ST_0601_SENSOR_RELATIVE_ELEVATION_ANGLE, getSensorRelativeElevationAngle},
    {St0601Tag::ST_0601_SENSOR_RELATIVE_ROLL_ANGLE, getSensorRelativeRollAngle},
    {St0601Tag::ST_0601_SLANT_RANGE, getSlantRange},
    {St0601Tag::ST_0601_TARGET_WIDTH, getTargetWidth},
    {St0601Tag::ST_0601_FRAME_CENTER_LATITUDE, getFrameCenterLatitude},
    {St0601Tag::ST_0601_FRAME_CENTER_LONGITUDE, getFrameCenterLongitude},
    {St0601Tag::ST_0601_FRAME_CENTER_ELEVATION, getFrameCenterElevation},
    {St0601Tag::ST_0601_OFFSET_CORNER_LATITUDE_POINT1, getOffsetCornerLatitudePoint1},
    {St0601Tag::ST_0601_OFFSET_CORNER_LONGITUDE_POINT1, getOffsetCornerLongitudePoint1},
    {St0601Tag::ST_0601_OFFSET_CORNER_LATITUDE_POINT2, getOffsetCornerLatitudePoint2},
    {St0601Tag::ST_0601_OFFSET_CORNER_LONGITUDE_POINT2, getOffsetCornerLongitudePoint2},
    {St0601Tag::ST_0601_OFFSET_CORNER_LATITUDE_POINT3, getOffsetCornerLatitudePoint3},
    {St0601Tag::ST_0601_OFFSET_CORNER_LONGITUDE_POINT3, getOffsetCornerLongitudePoint3},
    {St0601Tag::ST_0601_OFFSET_CORNER_LATITUDE_POINT4, getOffsetCornerLatitudePoint4},
    {St0601Tag::ST_0601_OFFSET_CORNER_LONGITUDE_POINT4, getOffsetCornerLongitudePoint4},
    {St0601Tag::ST_0601_ICING_DETECTED, getIcingDetected},
    {St0601Tag::ST_0601_WIND_DIRECTION, getWindDirection},
    {St0601Tag::ST_0601_WIND_SPEED, getWindSpeed},
    {St0601Tag::ST_0601_STATIC_PRESSURE, getStaticPressure},
    {St0601Tag::ST_0601_DENSITY_ALTITUDE, getDensityAltitude},
    {St0601Tag::ST_0601_OUTSIDE_AIR_TEMPERATURE, getOutsideAirTemperature},
    {St0601Tag::ST_0601_TARGET_LOCATION_LATITUDE, getTargetLocationLatitude},
    {St0601Tag::ST_0601_TARGET_LOCATION_LONGITUDE, getTargetLocationLongitude},
    {St0601Tag::ST_0601_TARGET_LOCATION_ELEVATION, getTargetLocationElevation},
    {St0601Tag::ST_0601_TARGET_TRACK_GATE_WIDTH, getTargetTrackGateWidth},
    {St0601Tag::ST_0601_TARGET_TRACK_GATE_HEIGHT, getTargetTrackGateHeight},
    {St0601Tag::ST_0601_TARGET_ERROR_ESTIMATE_CE90, getTargetErrorEstimateCe90},
    {St0601Tag::ST_0601_TARGET_ERROR_ESTIMATE_LE90, getTargetErrorEstimateLe90},
    {St0601Tag::ST_0601_GENERIC_FLAG_DATA, getGenericFlagData},
    {St0601Tag::ST_0601_SECURITY_LOCAL_SET, getSecurityLocalSet},
    {St0601Tag::ST_0601_DIFFERENTIAL_PRESSURE, getDifferentialPressure},
    {St0601Tag::ST_0601_PLATFORM_ANGLE_OF_ATTACK, getPlatformAngleOfAttack},
    {St0601Tag::ST_0601_PLATFORM_VERTICAL_SPEED, getPlatformVerticalSpeed},
    {St0601Tag::ST_0601_PLATFORM_SIDESLIP_ANGLE, getPlatformSideslipAngle},
    {St0601Tag::ST_0601_AIRFIELD_BAROMETRIC_PRESSURE, getAirfieldBarometricPressure},
    {St0601Tag::ST_0601_AIRFIELD_ELEVATION, getAirfieldElevation},
    {St0601Tag::ST_0601_RELATIVE_HUMIDITY, getRelativeHumidity},
    {St0601Tag::ST_0601_PLATFORM_GROUND_SPEED, getPlatformGroundSpeed},
    {St0601Tag::ST_0601_GROUND_RANGE, getGroundRange},
    {St0601Tag::ST_0601_PLATFORM_FUEL_REMAINING, getPlatformFuelRemaining},
    {St0601Tag::ST_0601_PLATFORM_CALL_SIGN, getPlatformCallSign},
    {St0601Tag::ST_0601_WEAPON_LOAD, getWeaponLoad},
    {St0601Tag::ST_0601_WEAPON_FIRED, getWeaponFired},
    {St0601Tag::ST_0601_LASER_PRF_CODE, getLaserPrfCode},
    {St0601Tag::ST_0601_SENSOR_FIELD_OF_VIEW_NAME, getSensorFieldOfViewName},
    {St0601Tag::ST_0601_PLATFORM_MAGNETIC_HEADING, getPlatformMagneticHeading},
    {St0601Tag::ST_0601_UAS_DATALINK_LS_VERSION_NUMBER, getUasDatalinkLsVersionNumber},
    {St0601Tag::ST_0601_DEPRECATED, getDeprecated},
    {St0601Tag::ST_0601_ALTERNATE_PLATFORM_LATITUDE, getAlternatePlatformLatitude},
    {St0601Tag::ST_0601_ALTERNATE_PLATFORM_LONGITUDE, getAlternatePlatformLongitude},
    {St0601Tag::ST_0601_ALTERNATE_PLATFORM_ALTITUDE, getAlternatePlatformAltitude},
    {St0601Tag::ST_0601_ALTERNATE_PLATFORM_NAME, getAlternatePlatformName},
    {St0601Tag::ST_0601_ALTERNATE_PLATFORM_HEADING, getAlternatePlatformHeading},
    {St0601Tag::ST_0601_EVENT_START_TIME_UTC, getEventStartTimeUtc},
    {St0601Tag::ST_0601_RVT_LOCAL_SET, getRvtLocalSet},
    {St0601Tag::ST_0601_VMTI_LOCAL_SET, getVmtiLocalSet},
    {St0601Tag::ST_0601_SENSOR_ELLIPSOID_HEIGHT, getSensorEllipsoidHeight},
    {St0601Tag::ST_0601_ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT, getAlternatePlatformEllipsoidHeight},
    {St0601Tag::ST_0601_OPERATIONAL_MODE, getOperationalMode},
    {St0601Tag::ST_0601_FRAME_CENTER_HEIGHT_ABOVE_ELLIPSOID, getFrameCenterHeightAboveEllipsoid},
    {St0601Tag::ST_0601_SENSOR_NORTH_VELOCITY, getSensorNorthVelocity},
    {St0601Tag::ST_0601_SENSOR_EAST_VELOCITY, getSensorEastVelocity},
    {St0601Tag::ST_0601_IMAGE_HORIZON_PIXEL_PACK, getImageHorizonPixelPack},
    {St0601Tag::ST_0601_CORNER_LATITUDE_POINT1_FULL, getCornerLatitudePoint1Full},
    {St0601Tag::ST_0601_CORNER_LONGITUDE_POINT1_FULL, getCornerLongitudePoint1Full},
    {St0601Tag::ST_0601_CORNER_LATITUDE_POINT2_FULL, getCornerLatitudePoint2Full},
    {St0601Tag::ST_0601_CORNER_LONGITUDE_POINT2_FULL, getCornerLongitudePoint2Full},
    {St0601Tag::ST_0601_CORNER_LATITUDE_POINT3_FULL, getCornerLatitudePoint3Full},
    {St0601Tag::ST_0601_CORNER_LONGITUDE_POINT3_FULL, getCornerLongitudePoint3Full},
    {St0601Tag::ST_0601_CORNER_LATITUDE_POINT4_FULL, getCornerLatitudePoint4Full},
    {St0601Tag::ST_0601_CORNER_LONGITUDE_POINT4_FULL, getCornerLongitudePoint4Full},
    {St0601Tag::ST_0601_PLATFORM_PITCH_ANGLE_FULL, getPlatformPitchAngleFull},
    {St0601Tag::ST_0601_PLATFORM_ROLL_ANGLE_FULL, getPlatformRollAngleFull},
    {St0601Tag::ST_0601_PLATFORM_ANGLE_OF_ATTACK_FULL, getPlatformAngleOfAttackFull},
    {St0601Tag::ST_0601_PLATFORM_SIDESLIP_ANGLE_FULL, getPlatformSideslipAngleFull},
    {St0601Tag::ST_0601_MIIS_CORE_IDENTIFIER, getMiisCoreIdentifier},
    {St0601Tag::ST_0601_SAR_MOTION_IMAGERY_LOCAL_SET, getSarMotionImageryLocalSet},
    {St0601Tag::ST_0601_TARGET_WIDTH_EXTENDED, getTargetWidthExtended},
    {St0601Tag::ST_0601_RANGE_IMAGE_LOCAL_SET, getRangeImageLocalSet},
    {St0601Tag::ST_0601_GEO_REGISTRATION_LOCAL_SET, getGeoRegistrationLocalSet},
    {St0601Tag::ST_0601_COMPOSITE_IMAGING_LOCAL_SET, getCompositeImagingLocalSet},
    {St0601Tag::ST_0601_SEGMENT_LOCAL_SET, getSegmentLocalSet},
    {St0601Tag::ST_0601_AMEND_LOCAL_SET, getAmendLocalSet},
    {St0601Tag::ST_0601_SDCC_FLP, getSdccFlp},
    {St0601Tag::ST_0601_DENSITY_ALTITUDE_EXTENDED, getDensityAltitudeExtended},
    {St0601Tag::ST_0601_SENSOR_ELLIPSOID_HEIGHT_EXTENDED, getSensorEllipsoidHeightExtended},
    {St0601Tag::ST_0601_ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT_EXTENDED, getAlternatePlatformEllipsoidHeightExtended},
    {St0601Tag::ST_0601_STREAM_DESIGNATOR, getStreamDesignator},
    {St0601Tag::ST_0601_OPERATIONAL_BASE, getOperationalBase},
    {St0601Tag::ST_0601_BROADCAST_SOURCE, getBroadcastSource},
    {St0601Tag::ST_0601_RANGE_TO_RECOVERY_LOCATION, getRangeToRecoveryLocation},
    {St0601Tag::ST_0601_TIME_AIRBORNE, getTimeAirborne},
    {St0601Tag::ST_0601_PROPULSION_UNIT_SPEED, getPropulsionUnitSpeed},
    {St0601Tag::ST_0601_PLATFORM_COURSE_ANGLE, getPlatformCourseAngle},
    {St0601Tag::ST_0601_ALTITUDE_AGL, getAltitudeAgl},
    {St0601Tag::ST_0601_RADAR_ALTIMETER, getRadarAltimeter},
    {St0601Tag::ST_0601_CONTROL_COMMAND, getControlCommand},
    {St0601Tag::ST_0601_CONTROL_COMMAND_VERIFICATION, getControlCommandVerification},
    {St0601Tag::ST_0601_SENSOR_AZIMUTH_RATE, getSensorAzimuthRate},
    {St0601Tag::ST_0601_SENSOR_ELEVATION_RATE, getSensorElevationRate},
    {St0601Tag::ST_0601_SENSOR_ROLL_RATE, getSensorRollRate},
    {St0601Tag::ST_0601_ON_BOARD_MI_STORAGE_PERCENT_FULL, getOnBoardMiStoragePercentFull},
    {St0601Tag::ST_0601_ACTIVE_WAVELENGTH_LIST, getActiveWavelengthList},
    {St0601Tag::ST_0601_COUNTRY_CODES, getCountryCodes},
    {St0601Tag::ST_0601_NUMBER_OF_NAVSATS_IN_VIEW, getNumberOfNavsatsInView},
    {St0601Tag::ST_0601_POSITIONING_METHOD_SOURCE, getPositioningMethodSource},
    {St0601Tag::ST_0601_PLATFORM_STATUS, getPlatformStatus},
    {St0601Tag::ST_0601_SENSOR_CONTROL_MODE, getSensorControlMode},
    {St0601Tag::ST_0601_SENSOR_FRAME_RATE_PACK, getSensorFrameRatePack},
    {St0601Tag::ST_0601_WAVELENGTHS_LIST, getWavelengthsList},
    {St0601Tag::ST_0601_TARGET_ID, getTargetId},
    {St0601Tag::ST_0601_AIRBASE_LOCATIONS, getAirbaseLocations},
    {St0601Tag::ST_0601_TAKE_OFF_TIME, getTakeOffTime},
    {St0601Tag::ST_0601_TRANSMISSION_FREQUENCY, getTransmissionFrequency},
    {St0601Tag::ST_0601_ON_BOARD_MI_STORAGE_CAPACITY, getOnBoardMiStorageCapacity},
    {St0601Tag::ST_0601_ZOOM_PERCENTAGE, getZoomPercentage},
    {St0601Tag::ST_0601_COMMUNICATIONS_METHOD, getCommunicationsMethod},
    {St0601Tag::ST_0601_LEAP_SECONDS, getLeapSeconds},
    {St0601Tag::ST_0601_CORRECTION_OFFSET, getCorrectionOffset},
    {St0601Tag::ST_0601_PAYLOAD_LIST, getPayloadList},
    {St0601Tag::ST_0601_ACTIVE_PAYLOADS, getActivePayloads},
    {St0601Tag::ST_0601_WEAPON_STORES, getWeaponStores},
    {St0601Tag::ST_0601_WAYPOINT_LIST, getWaypointList},
};

St0601Getter::St0601Getter() : getters(defaultGetters) {}
St0601Getter::~St0601Getter() {}

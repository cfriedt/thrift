namespace cpp org.misb

enum GenericFlagData {
	LASER_OFF = 1
	AUTO_TRACK_OFF = 2
	IR_POLARITY_WHITE_HOT = 4
	ICING_STATUS_NONE = 8
	SLANT_RANGE_CALCULATED = 16
	IMAGE_VALID = 32
}

struct SecurityLocalSet {
}

enum SensorFieldOfViewName {
	ULTRANARROW
	NARROW
	MEDIUM
	WIDE
	ULTRAWIDE
	NARROW_MEDIUM
	ULTRANARROW_2X
	ULTRANARROW_4X
}

struct RvtLocalSet {
}
struct VmtiLocalSet {
}
enum OperationalMode {
	OTHER
	OPERATIONAL
	TRAINING
	EXERCISE
	MAINTENANCE
	TEST
} (
	MaxLength = "1"
)

struct ImageHorizontalPixelPack {
} (
	OmitKey
	OmitLength
)

struct SarMotionImageryLocalSet {
}
struct RangeImageLocalSet {
}
struct GeoRegistrationLocalSet {
}
struct CompositeImagingLocalSet {
}
struct SegmentLocalSet {
}
struct AmendLocalSet {
}
struct SdccFlp {
} (
	OmitKey
	OmitLength
	LastFieldHasVariableLength
)

struct ControlCommand {
} (
	OmitKey
	OmitLength
)

struct ControlCommandVerificationList {
} (
	OmitKey
	OmitLength
)

struct ActiveWavelengthList {
} (
	OmitKey
	OmitLength
)

struct CountryCodes {
} (
	OmitKey
)

enum PlatformStatus {
	ACTIVE
	PRE_FLIGHT
	PRE_FLIGHT_TAXIING
	RUN_UP
	TAKE_OFF
	INGRESS
	MANUAL_OPERATION
	AUTOMATED_ORBIT
	TRANSITIONING
	EGRESS
	LANDING
	LANDED_TAXIING
	LANDED_PARKED
}

enum SensorControlMode {
	OFF
	HOME_POSITION
	UNCONTROLLED
	MANUAL_CONTROL
	CALIBRATING
	AUTO_HOLDING_POSITION
	AUTO_TRACKING
}

struct SensorFrameRatePack {
} (
	OmitKey
	OmitLength
)

struct WaveLengthList {
} (
	OmitKey
)

struct AirbaseLocations {
} (
	OmitKey
)

struct PayloadList {
} (
	OmitKey
)

struct WeaponStores {
} (
	OmitKey
)

struct WaypointList {
} (
	OmitKey
)

struct UasDataLinkLocalSet {
	// precisionTimeStamp must appear first
	2: required i64 precisionTimeStamp (
		Unsigned
	)
	3: optional string missionId (
		MaxLength = "127"
	)
	4: optional string platformTailNumber (
		MaxLength = "127"
	)
	5: optional double platformHeadingAngle (
		SinglePrecision
		IMAPB = "0, 360, 2"
	)
	6: optional double platformPitchAngle (
		SinglePrecision
		IMAPB = "-20, 20, 2"
		SpecialValues = "0x8000, Out of Range"
	)
	7: optional double platformRollAngle (
		SinglePrecision
		IMAPB = "-50, 50, 2"
		SpecialValues = "0x8000, Out of Range"
	)
	8: optional i8 platformTrueAirspeed (
		Unsigned
	)
	9: optional i8 platformIndicatedAirspeed (
		Unsigned
	)
	10: optional string platformDesignation (
		MaxLength = "127"
	)
	11: optional string imageSensorSource (
		MaxLength = "127"
	)
	12: optional string imageCoordinateSystem (
		MaxLength = "127"
	)
	13: optional double sensorLattitude (
		IMAPB = "-90, 90, 4"
		SpecialValues = "0x80000000, Reserved"
	)
	14: optional double sensorLongitude (
		IMAPB = "-180, 180, 4"
		SpecialValues = "0x80000000, Reserved"
	)
	15: optional double sensorTrueAltitude (
		SinglePrecision
		IMAPB = "-900, 19000, 2"
	)
	16: optional double sensorHorizontalFieldOfView (
		SinglePrecision
		IMAPB = "0, 180, 2"
	),	
	17: optional double sensorVerticalFieldOfView (
		SinglePrecision
		IMAPB = "0, 180, 2"
	)
	18: optional double sensorRelativeAzimuthAngle (
		IMAPB = "0, 360, 4"
	)
	19: optional double sensorRelativeElevationAngle (
		IMAPB = "-180, 180, 4"
	)
	20: optional double sensorRelativeRollAngle (
		SinglePrecision
		IMAPB = "0, 10000, 2"
	)
	21: optional double slantRange (
		IMAPB = "0, 5000000, 4"
	)
	22: optional double targetWidth (
		SinglePrecision
		IMAPB = "0, 10000, 2"
	)
	23: optional double frameCenterLatitude (
		IMAPB = "-90, 90, 4"
	)
	24: optional double frameCenterLongitude (
		IMAPB = "-180, 180, 4"
	)
	25: optional double frameCenterElevation (
		SinglePrecision
		IMAPB = "-900, 19000, 2"
	)
	26: optional double offsetCornerLatitudePoint1 (
		SinglePrecision
		IMAPB = "-0.075, 0.075, 2"
		SpecialValues = "0x8000, N/A (Off-Earth)"
	)
	27: optional double offsetCornerLongitudePoint1 (
		SinglePrecision
		IMAPB = "-0.075, 0.075, 2"
		SpecialValues = "0x8000, N/A (Off-Earth)"
	)
	28: optional double offsetCornerLatitudePoint2 (
		SinglePrecision
		IMAPB = "-0.075, 0.075, 2"
		SpecialValues = "0x8000, N/A (Off-Earth)"
	)
	29: optional double offsetCornerLongitudePoint2 (
		SinglePrecision
		IMAPB = "-0.075, 0.075, 2"
		SpecialValues = "0x8000, N/A (Off-Earth)"
	)
	30: optional double offsetCornerLatitudePoint3 (
		SinglePrecision
		IMAPB = "-0.075, 0.075, 2"
		SpecialValues = "0x8000, N/A (Off-Earth)"
	)
	31: optional double offsetCornerLongitudePoint3 (
		SinglePrecision
		IMAPB = "-0.075, 0.075, 2"
		SpecialValues = "0x8000, N/A (Off-Earth)"
	)
	32: optional double offsetCornerLatitudePoint4 (
		SinglePrecision
		IMAPB = "-0.075, 0.075, 2"
		SpecialValues = "0x8000, N/A (Off-Earth)"
	)
	33: optional double offsetCornerLongitudePoint4 (
		SinglePrecision
		IMAPB = "-0.075, 0.075, 2"
		SpecialValues = "0x8000, N/A (Off-Earth)"
	)
	34: optional bool icingDetected
	35: optional double windDirection (
		SinglePrecision
		IMAPB = "0, 360, 2"
	)
	36: optional double windSpeed (
		SinglePrecision
		IMAPB = "0, 100, 1"
	)
	37: optional double staticPressure (
		SinglePrecision
		IMAPB = "0, 5000, 2"
	)
	38: optional i16 densityAltitude (
		SinglePrecision
		IMAPB = "-900, 19000, 2"
		Offset = "900"
	)
	39: optional i8 outsideAirTemperature
	40: optional double targetLocationLatitude (
		IMAPB = "-90, 90, 4"
		SpecialValues = "0x80000000, N/A (Off-Earth)"
	)
	41: optional double targetLocationLongitude(
		IMAPB = "-180, 180, 4"
		SpecialValues = "0x80000000, N/A (Off-Earth)"
	)
	42: optional double targetLocationElevation (
		SinglePrecision
		IMAPB = "-900, 19000, 2"
		Offset = "-900"
	)
	43: optional i16 targetTrackGateWidth (
		Unsigned
		MaxValue = "510"
		MaxLength = "1"
		ScaleFactor = "2"
	),	
	44: optional i16 targetTrackGateHeight (
		Unsigned
		MaxValue = "510"
		MaxLength = "1"
		ScaleFactor = "2"
	)
	45: optional double targetErrorEstimateCe90 (
		SinglePrecision
		IMAPB = "0, 4095, 2"
	)
	46: optional double targetErrorEstimateLe90 (
		SinglePrecision
		IMAPB = "0, 4095, 2"
	)
	47: optional GenericFlagData genericFlagData (
		MaxLength = "1"
	),	
	48: optional SecurityLocalSet securityLocalSet
	49: optional double differentialPressure (
		SinglePrecision
		IMAPB = "0, 5000, 2"
	),	
	50: optional double platformAngleOfAttack (
		SinglePrecision
		IMAPB = "-20, 20, 2"
		SpecialValues = "0x8000, Out of Range"
	)
	51: optional double platformVerticalSpeed (
		SinglePrecision
		IMAPB = "-180, 180, 2"
		SpecialValues = "0x8000, Out of Range"
	)
	52: optional double platformSideslipAngle (
		SinglePrecision
		IMAPB = "-20, 20, 2"
		SpecialValues = "0x8000, Out of Range"
	)
	53: optional double airfieldBarometricPressure (
		SinglePrecision
		IMAPB = "0, 5000, 2"
		SpecialValues = "0x8000, Out of Range"
	)
	54: optional double airfieldElevation (
		SinglePrecision
		IMAPB = "-900, 19000, 2"
		Offset = "-900"
	)
	55: optional double relativeHumidity (
		SinglePrecision
		IMAPB = "0, 100, 1"
	)
	56: optional i8 platformGroundSpeed (
		Unsigned
	)
	57: optional double groundRange (
		IMAPB = "0, 5000000, 4"
	)
	58: optional i16 platformFuelRemaining (
		SinglePrecision
		IMAPB = "0, 10000, 2"
	)
	59: optional string platformCallSign (
		MaxLength = "127"
	)
	60: optional i16 weaponLoad (
		Unsigned
	)
	61: optional i8 weaponFired (
		Unsigned
	),	
	62: optional i16 laserPrfCode (
		Unsigned
	)
	63: optional SensorFieldOfViewName sensorFieldOfViewName (
		MaxLength = "1"
	)
	64: optional double platformMagneticHeading (
		SinglePrecision
		IMAPB = "0, 360, 2"
	)
	65: required i8 uasDatalinkLsVersionNumber (
		Unsigned
	)
	66: optional i8 invalid (
		Deprecated
	)
	67: optional double alternatePlatformLatitude (
		IMAPB = "-90, 90, 4"
		SpecialValues = "0x80000000, Reserved"
	),	
	68: optional double alternatePlatformLongitude (
		IMAPB = "-180, 180, 4"
		SpecialValues = "0x80000000, Reserved"
	)
	69: optional double alternatePlatformAltitude (
		SinglePrecision
		IMAPB = "-900 19000, 4"
		Offset = "-900"
	)
	70: optional string alternatePlatformName (
		MaxLength = "127"
	)
	71: optional double alternatePlatformHeading (
		SinglePrecision
		IMAPB = "0, 360, 2"
	)
	72: optional i64 eventStartTimeUtc (
		Unsigned
	)
	73: optional RvtLocalSet rvtLocalSet
	74: optional VmtiLocalSet vmtiLocalSet
	75: optional double sensorEllipsoidHeight (
		SinglePrecision
		IMAPB = "-900 19000, 2"
		Offset = "-900"
	)
	76: optional double alternatePlatformEllipsoidHeight (
		SinglePrecision
		IMAPB = "-900 19000, 2"
		Offset = "-900"
	)
	77: optional OperationalMode operationalMode (
		MaxLength = "1"
	)
	78: optional double frameCenterHeightAboveEllipsoid (
		SinglePrecision
		IMAPB = "-900, 19000, 2"
		Offset = "-900"
	),	
	79: optional double sensorNorthVelocity (
		SinglePrecision
		IMAPB = "-327, 327, 2"
		SpecialValues = "0x8000, Out of Range"
	)
	80: optional double sensorEastVelocity (
		SinglePrecision
		IMAPB = "-327, 327, 2"
		SpecialValues = "0x8000, Out of Range"
	)
	81: optional ImageHorizontalPixelPack imageHorizonPixelPack
	82: optional double cornerLatitudePoint1_Full (
		IMAPB = "-90, 90, 4"
		SpecialValues = "0x80000000, N/A (Off-Earth)"
	)
	83: optional double cornerLongitudePoint1_Full (
		IMAPB = "-180, 180, 4"
		SpecialValues = "0x80000000, N/A (Off-Earth)"
	)
	84: optional double cornerLatitudePoint2_Full (
		IMAPB = "-90, 90, 4"
		SpecialValues = "0x80000000, N/A (Off-Earth)"
	)
	85: optional double cornerLongitudePoint2_Full (
		IMAPB = "-180, 180, 4"
		SpecialValues = "0x80000000, N/A (Off-Earth)"
	)
	86: optional double cornerLatitudePoint3_Full(
		IMAPB = "-90, 90, 4"
		SpecialValues = "0x80000000, N/A (Off-Earth)"
	)
	87: optional double cornerLongitudePoint3_Full (
		IMAPB = "-180, 180, 4"
		SpecialValues = "0x80000000, N/A (Off-Earth)"
	)
	88: optional double cornerLatitudePoint4_Full(
		IMAPB = "-90, 90, 4"
		SpecialValues = "0x80000000, N/A (Off-Earth)"
	)
	89: optional double cornerLongitudePoint4_Full (
		IMAPB = "-180, 180, 4"
		SpecialValues = "0x80000000, N/A (Off-Earth)"
	)
	90: optional double platformPitchAngleFull (
		IMAPB = "-90, 90, 4"
		SpecialValues = "0x80000000, Out of Range"
	)
	91: optional double platformRollAngleFull (
		IMAPB = "-90, 90, 4"
		SpecialValues = "0x80000000, Out of Range"
	)
	92: optional double platformAngleOfAttackFull (
		IMAPB = "-90, 90, 4"
		SpecialValues = "0x80000000, Out of Range"
	)
	93: optional double platformSideslipAngleFull (
		IMAPB = "-180, 180, 4"
		SpecialValues = "0x80000000, Out of Range"
	)
	94: optional binary miisCoreIdentifier (
		MaxLength = "50"
	)
	95: optional SarMotionImageryLocalSet sarMotionImageryLocalSet
	96: optional double targetWidthExtended (
		IMAPB = "0, 1500000, 4"
	),	
	97: optional RangeImageLocalSet rangeImageLocalSet
	98: optional GeoRegistrationLocalSet geoRegistrationLocalSet
	99: optional CompositeImagingLocalSet compositeImagingLocalSet
	100: optional SegmentLocalSet segmentLocalSet
	101: optional AmendLocalSet amendLocalSet
	102: optional SdccFlp sdccFlp
	103: optional double densityAltitudeExtended (
		IMAPB = "-900, 40000, 4"
		MaxLength = "8"
	)
	104: optional double sensorEllipsoidHeightExtended (
		IMAPB = "-900, 40000, 4"
		MaxLength = "8"
	)
	105: optional double  alternatePlatformEllipsoidHeightExtended (
		IMAPB = "-900, 40000, 4"
		MaxLength = "8"
	)
	106: optional string streamDesignator (
		MaxLength = "127"
	)
	107: optional string operationalBase (
		MaxLength = "127"
	)
	108: optional string broadcastSource (
		MaxLength = "127"
	)
	109: optional double rangeToRecoveryLocation (
		SinglePrecision
		IMAPB = "0, 21000, 4"
	)
	110: optional i32 timeAirborne (
		Unsigned
	)
	111: optional i32 propulsionUnitSpeed (
		Unsigned
	)
	112: optional double platformCourseAngle (
		IMAPA = "0, 360, 0.016625"
		MaxLength = "8"
	)
	113: optional double altitudeAgl (
		IMAPA = "-900, 40000, 0.007"
		MaxLength = "4"
	),	
	114: optional double radarAltimeter (
		IMAPA = "-900, 40000, 0.007"
		MaxLength = "4"
	),	
	115: optional ControlCommand controlCommand
	116: optional ControlCommandVerificationList controlCommandVerificationList
	117: optional double sensorAzimuthRate (
		SinglePrecision
		IMAPA = "-1000, 1000, 0.000244"
		MaxLength = "4"
	)
	118: optional double sensorElevationRate (
		SinglePrecision
		IMAPA = "-1000, 1000, 0.000244"
		MaxLength = "4"
	)
	119: optional double sensorRollRate (
		SinglePrecision
		IMAPA = "-1000, 1000, 0.000244"
		MaxLength = "4"
	)
	120: optional double onBoardMiStoragePercentFull (
		SinglePrecision
		IMAPA = "0, 100, 0.000015"
		MaxLength = "3"
	)
	121: optional ActiveWavelengthList activeWavelengthList
	122: optional CountryCodes countryCodes
	123: optional i8 numberOfNavsatsInView (
		Unsigned
	)
	124: optional i8 positioningMethodSource (
		Unsigned
	)
	125: optional PlatformStatus platformStatus
	126: optional SensorControlMode sensorControlMode
	127: optional SensorFrameRatePack sensorFrameRatePack
	128: optional WaveLengthList wavelengthsList
	129: optional string targetId (
		MaxLength = "32"
	)
	130: optional AirbaseLocations airbaseLocations (
		MaxLength = "24"
	)
	131: optional i64 takeOffTime (
		Unsigned
	)
	132: optional double transmissionFrequency (
		IMAPA = "1, 99999, 0.015625"
		MaxLength = "8"
	)
	133: optional i32 onBoardMiStorageCapacity (
		Unsigned
	)
	134: optional double zoomPercentage (
		SinglePrecision
		IMAPA = "0, 100, 0.0039"
		MaxLength = "4"
	)
	135: optional string communicationsMethod (
		MaxLength = "127"
	)
	136: optional i32 leapSeconds (
		MaxLength = "4"
	)
	137: optional i64 correctionOffset (
		MaxLength = "8"
	)
	138: optional PayloadList payloadList
	139: optional binary activePayloads
	140: optional WeaponStores weaponStores
	141: optional WaypointList waypointList
	// checksum must appear last
	1: required i16 checksum (
		Unsigned
	)
}
service St060115 {
	oneway void update( 1: UasDataLinkLocalSet message )
}
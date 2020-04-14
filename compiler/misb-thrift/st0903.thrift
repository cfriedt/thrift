namespace cpp org.misb

struct LocationTruncationPack {
	1: required double latitude (
		IMAPB = "-90, 90, 4"
	)
	2: optional double longitude (
		IMAPB = "-180, 180, 4"
	)
	3: optional double height (
		IMAPB = "-900, 19000, 2"
	)
	4: optional double sigmaEast (
		IMAPB = "0, 650, 2"
	)
	5: optional double sigmaNorth (
		IMAPB = "0, 650, 2"
	)
	6: optional double sigmaUp (
		IMAPB = "0, 650, 2"
	)
	7: optional double rhoEastNorth (
		IMAPB = "-1, 1, 2"	
	) 
	8: optional double rhoEastUp (
		IMAPB = "-1, 1, 2"	
	) 
	9: optional double rhoNorthUp (
		IMAPB = "-1, 1, 2"	
	) 
} (
	DLP
) 
typedef LocationTruncationPack Location

struct BoundarySeries {
	// XXX: @CJF: 20200323: list is not yet supported
	//1: required list<Location> locationPack
	1: required binary locationPack
} (
	VLP
)
typedef BoundarySeries Boundary

struct FPAIndex {
	1: required i8 first
	2: required i8 second
} (
	DLP
)

struct VMaskLocalSet {
	// XXX: @CJF: 20200323: list ist not yet supported
	//1: list<i32> polygon
	1: optional binary polygon
	/*
	 * This is an "array of unsigned integers" and has variable length, but it
	 * does not specify what the bit-width of the integers is.
	 */
	2: optional binary bitMask
}

struct VObjectLocalSet {
	1: optional string ontology
	2: optional string ontologyClass
}

struct VFeatureLocalSet {
	1: optional string schema
	2: optional string schemaFeature
}

struct VelocityTrucationPack {
	1: required double eastComponent (
		IMAPB = "-900, 900, 2"
	)
	2: optional double northComponent (
		IMAPB = "-900, 900, 2"
	)
	3: optional double upComponent (
		IMAPB = "-900, 900, 2"
	)
	4: optional double sigmaEast (
		IMAPB = "0, 650, 2"
	)
	5: optional double sigmaNorth (
		IMAPB = "0, 650, 2"
	)
	6: optional double sigmaUp (
		IMAPB = "0, 650, 2"
	)
	7: optional double rhoEastNorth (
		IMAPB = "-1, 1, 2"
	)
	8: optional double rhoEastUp (
		IMAPB = "-1, 1, 2"
	)
	9: optional double rhoNorthUp (
		IMAPB = "-1, 1, 2"
	)
} (
	DLP
) 
typedef VelocityTrucationPack Velocity

struct AccelerationTrucationPack {
	1: required double eastComponent (
		IMAPB = "-900, 900, 2"
	)
	2: optional double northComponent (
		IMAPB = "-900, 900, 2"
	)
	3: optional double upComponent (
		IMAPB = "-900, 900, 2"
	)
	4: required double sigmaEast (
		IMAPB = "0, 650, 2"
	)
	5: optional double sigmaNorth (
		IMAPB = "0, 650, 2"
	)
	6: optional double sigmaUp (
		IMAPB = "0, 650, 2"
	)
	7: optional double rhoEastNorth (
		IMAPB = "-1, 1, 2"
	)
	8: optional double rhoEastUp (
		IMAPB = "-1, 1, 2"
	)
	9: optional double rhoNorthUp (
		IMAPB = "-1, 1, 2"
	)
} (
	DLP
)
typedef AccelerationTrucationPack Acceleration

struct VTrackerLocalSet {
	// this is a Uint128, but thrift has no concept of that
	1: optional binary trackId (
		FixedLength = "16"
	)
	2: optional i8 detectionStatus
	3: optional i64 startTimeStamp (
		Unsigned
		MaxLength = "8"
	)
	4: optional i64 endTimeStamp (
		Unsigned
		MaxLength = "8"
	)
	5: optional Boundary boundingBox
	6: optional string algorithm
	7: optional i8 confidence (
		Unsigned
	)
	8: optional i16 numberOfTrackPoints (
		Unsigned
		MaxLength = "2"
	)
	// XXX: @CJF: 20200323: list is not yet supported
	//9: optional list<Location> locus
	9: optional binary locus
	10: optional Velocity velocity
	11: optional Acceleration acceleration
}

struct VTargetPack {
	// must come first in the VTargetPack.  
	// 32767 is the highest number supported by thrift for enumerating fields 
	32767: required i32 targetIdNumber (
		Unsigned
		BEROID
		MinValue = "1"
		MaxValue = "2097151"
		// This field sticks out like a sore thumb, because it breaks most of
		// MISB's otherwise consistent KLV, DLP, VLP, and FLP rules. 
		SoreThumb
	)
	1: optional i64 targetCentroidPixelNumber (
		Unsigned
		MaxLength = "6"
	)
	2: optional i64 boundingBoxTopLeftPixelNumber (
		Unsigned
		MaxLength = "6"
	)
	3: optional i64 boundingBoxBottomRightPixelNumber (
		Unsigned
		MaxLength = "6"
	)
	4: optional i8 targetPriority (
		Unsigned
	)
	5: optional i8 targetConfidenceLevel (
		Unsigned
	)
	6: optional i16 newDetectionFlagTargetHistory (
		Unsigned
		MaxLength = "2"
	)
	7: optional i8 percentageOfTargetPixels (
		Unsigned
	)
	8: optional i32 targetColor (
		Unsigned
		FixedLength = "3"
	)
	9: optional i32 targetIntensity (
		Unsigned
		MaxLength = "3"
	)
	10: optional double targetLocationLatitudeOffset (
		IMAPB = "-19.2, 19.2, 3"
	)
	11: optional double targetLocationLongitudeOffset (
		IMAPB = "-19.2, 19.2, 3"
	)
	12: optional 	double targetHeight (
		IMAPB = "-900, 19000, 2"
	)
	13: optional double boundingBoxTopLeftLatitudeOffset (
		IMAPB = "-19.2, 19.2, 3"
	)
	14: optional double boundingBoxTopLeftLongitudeOffset (
		IMAPB = "-19.2, 19.2, 3"
	)
	15: optional double boundingBoxBottomRightLatitudeOffset (
		IMAPB = "-19.2, 19.2, 3"
	)
	16: optional double boundingBoxBottomRightLongitudeOffset (
		IMAPB = "-19.2, 19.2, 3"
	)
	17: optional Location targetLocation
	18: optional Boundary targetBoundary
	19: optional i32 targetCentroidPixelRow (
		Unsigned
		MaxLength = "4"
	)
	20: optional i32 targetCentroidPixelColumn (
		Unsigned
		MaxLength = "4"
	)
	21: optional FPAIndex fpaIndex
	101: optional VMaskLocalSet vMaskLs
	102: optional VObjectLocalSet vObjectLs
	103: optional VFeatureLocalSet vFeatureLs
	104: optional VTrackerLocalSet vTrackerLs
}

struct VMTILocalSet {
	1: optional i16 checksum (
		Unsigned
	)
	2: optional i64 precisionTimeStamp (
		Unsigned
	)
	3: optional string vmtiSystemNameDescription (
		MaxLength = "32"
	)
	4: optional i16 vmtiLsVersionNumber (
		Unsigned
	)
	5: optional i32 totalNumberOfTargetsDetectedInTheFrame (
		Unsigned
		MaxLength = "3"
	)
	6: optional i32 numberOfReportedTargets (
		Unsigned
		MaxLength = "3"
	)
	7: optional i32 motionImageryFrameNumber (
		Unsigned
		MaxLength = "3"		
	)
	8: optional i32 frameWidth (
		Unsigned
		MaxLength = "3"		
	)
	9: optional i32 frameHeight (
		Unsigned
		MaxLength = "3"		
	)
	10: optional string vmtiSourceSensor (
		MaxLength = "127"		
	)
	11: optional double vmtiSensorHorizontalFieldOfView (
		IMAPB = "0, 180, 2"
	)
	12: optional double vmtiSensorVerticalFieldOfView (
		IMAPB = "0, 180, 2"
	)
	// Typically a Core MIIS Identifier
	13: optional binary motionImageryId
	
	// XXX: @CJF: 20200323: list is not yet supported
	//101: optional list<VTargetPack> vTargetSeries
	//101: optional binary vTargetSeries
	101: optional VTargetPack vTargetSeries
}

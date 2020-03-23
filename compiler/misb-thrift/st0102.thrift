namespace cpp org.misb

enum SecurityClassification {
	UNCLASSIFIED = 1
	RESTRICTED = 2
	CONFIDENTIAL = 3
	SECRET = 4
	TOP_SECRET = 5
}

enum ClassifyingCountryAndReleasingInstructionsCountryCodingMethod {
	ISO_3166_TWO_LETTER = 1
	ISO_3166_THREE_LETTER = 2
	FIPS_10_4_TWO_LETTER = 3
	FIPS_10_4_FOUR_LETTER = 4
	ISO_3166_NUMERIC = 5
	ISO_1059_TWO_LETTER = 6
	ISO_1059_THREE_LETTER = 7
	FIPS_10_4_MIXED = 10
	ISO_3166_MIXED = 11
	STANAG_1059_MIXED = 12
	GENC_TWO_LETTER = 13
	GENC_THREE_LETTER = 14
	GENC_NUMERIC = 15
	GENC_MIXED = 16
}

enum ObjectCountryCodingMethod {
	ISO_3166_TWO_LETTER = 1
	ISO_3166_THREE_LETTER = 2
	ISO_3166_NUMERIC = 3
	FIPS_10_4_TWO_LETTER = 4
	FIPS_10_4_FOUR_LETTER = 5
	ISO_1059_TWO_LETTER = 6
	ISO_1059_THREE_LETTER = 7
	GENC_TWO_LETTER = 13
	GENC_THREE_LETTER = 14
	GENC_NUMERIC = 15
	GENC_ADMINSUB = 64
}

struct SecurityLocalSet {
	1: required SecurityClassification securityClassification
	2: required ClassifyingCountryAndReleasingInstructionsCountryCodingMethod classifyingCountryAndReleasingInstructionsCountryCodingMethod
	3: required string classifyingCountry
	4: optional string securitySciShiInformation
	5: optional string caveats
	6: optional string releasingInstructions
    7: optional string classifiedBy
    8: optional string derivedFrom
    9: optional string classificationReason
    10: optional string declassificationDate (
    	FixedLength = "8"
    )
    11: optional string classificationAndMarkingSystem
    12: required ObjectCountryCodingMethod objectCountryCodingMethod
    13: required string objectCountryCodes
    14: optional string classificationComments
    22: required i16 version
    23: optional string classifyingCountryAndReleasingInstructionsCountryCodingMethodVersionDate
    24: optional string objectCountryCodingMethodVersionDate
}

Using Thrift for Encoding and Decoding MISB Data
================================================ 

List of Additional Annotations
------------------------------

Note, that annotations below describe modifications to data prior to being encoded.
It should be noted, that the inverse operation should be performed prior to decoding.

Also note, that the preferred style is to use as few annotations as possible. They
are present to denote special consideration. If an annotation is redundant, it should be
removed. E.g. annotating an `int8_t` value with `MaxValue = "127"` is redundant. Similarly,
annotating an `int8_t` with `MaxLength = "1"` is also redundant.  

| Annotation | Description |
|------------|-------------|
| `MaxValue = "N"` | Denotes the maximum value of a value |
| `MaxLength = "N"` | Denotes the maximum length (in bytes) of a value |
| `FixedLength = "N"` | Denotes the exact length (in bytes) of a value |
| `IMAPA = "X, Y, Z"` | Denotes the parameters used to encode a `double` argument as an integer (See [ST 1201.4](https://gwg.nga.mil/misb/docs/standards/ST1201.4.pdf)) |
| `IMAPB = "X, Y, Z"` | Denotes the parameters used to encode a `double` argument as an integer (See [ST 1201.4](https://gwg.nga.mil/misb/docs/standards/ST1201.4.pdf)) |
| `Offset = "N"` | Denotes the offset for encoding `double` argument as an integer (See [ST 1201.4](https://gwg.nga.mil/misb/docs/standards/ST1201.4.pdf)) |
| `ScaledUp = "N"` | Denotes that an integer value should be multiplied by `N` before encoding |
| `ScaledDown = "N"` | Denotes that an integer value should be divided by `N` before encoding |
| `SinglePrecision` | Hints to the code generator that it may use `float` instead of `double` for IEEE-754 representation |
| `Unsigned` | Hints to the code generator that it may use an appropriate unsigned type instead of the usual signed type |
| `DLP` | Data should be encoded as a Defined-Length Pack |
| `VLP` | Data should be encoded as a Variable-Length Pack |
| `FLP` | Data should be encoded as a Floating-Length Pack |
| `Deprecated` | Denotes that a particular tag should not be used in new designs |
| `SpecialValue = "0xabcd1234, This is (so) special"` | Denotes a special value for a particular MISB tag. This annotation can be specified more than once (double-check that) |

Caveats
-------

Please refer to [MISB](https://gwg.nga.mil/misb/st_pubs.html) for all official
documentation. 

There are a number of caveats to using Thrift to encode and decode MISB data,
but the "short" list is below. Specific parts of [ST 0601.15](https://gwg.nga.mil/misb/docs/standards/ST0601.15.pdf)
will be used as examples.

- UL Key and Length of the UAS Datalink Packet *SHALL NOT* be generated
    - MISB data is the payload of a UAS Datalink Packet
    - Wrapping and re-encoding MISB data is an independent process
- MISB encoding *SHALL NOT* be compatible with the Thrift binary encoding
- A given service *SHALL* be representative of a given standard
    - e.g. `St060115` is the service for the [ST 0601.15](https://gwg.nga.mil/misb/docs/standards/ST0601.15.pdf) standard 
    - the service *MUST* be limited to 1 RPC method
    - the method name *SHALL NOT* not be encoded in the Thrift message
    - the method *MUST* be oneway and have return type void
    - the method *MUST* have one parameter which is a top-level Local Set `struct`
    - the method *MUST NOT* throw a Thrift exception
- Thrift version info *SHALL NOT* be encoded in the Thrift message
- For a particular standard, the entire set of tags are considered part of one struct
    - required tags *MUST* use the `required` keyword
    - non-required tags *MUST* use the `optional` keyword
    - E.g. [ST 0601.15](https://gwg.nga.mil/misb/docs/standards/ST0601.15.pdf), there is one top-level Local Set `struct`, St060115Message, with 3 required fields
        - Checksum (tag 1)
        - Precision Time Stamp (tag 2)
        - UAS Datalink Local Set Version Number (tag 65)
    - At most 1 field *SHALL* use the `First` annotation (E.g. Precision Time Stamp)
    - At most 1 field *SHALL* use the `Last` annotation (E.g. Checksum)
- Tags (Keys) *SHALL* be BER-OID encoded, in accordance with MISB 
- Lengths (in terms of TLV / KLV) *SHALL* be BER encoded, in accordance with MISB
- Multibyte numeric values *SHALL* be encoded in network byte order, in accordance with MISB 

Use of the `struct` Keyword
-----------------------------

A Thrift `struct` *SHALL* be used to represent

- A top-level Local Set, such as UAS Datalink Local Set, [ST 0601.15](https://gwg.nga.mil/misb/docs/standards/ST0601.15.pdf), defined by MISB
- A Nested Local Set, such as RVT LS (Tag 73), with its own scoped Tags, defined by MISB 
- A Variable-Length Pack (annotated `VLP`)
    - order is fixed / known a-priori
    - tags are known a-priori
    - internal tags omitted (known a-priori)
    - internal lengths mandatory
    - all values not required (even if length is 0 for not-present values)
- A Defined-Length Pack (annotated `DLP`)
    - order and length are fixed / known a-priori
    - tags are known a-priori
    - internal tags omitted (known a-priori)
    - internal lengths omitted (known a-priori)
    - all values required

Use of Container Types
----------------------

Aside from the `struct` keyword, Thrift also supports the parameterized `list<T>, `set<T>`, and `map<K,V>` types.

A `list<T>` *SHALL* be encoded as a `DLP` if all of its (possibly nested) members are fixed-size values or have lengths that are inherent in the value itself (e.g. a BER or BER-OID encoded value).

A `list<T>` *SHALL* be encoded as a `VLP` if any of its (possibly nested) members are variable-size values and do not encode the length inherently.

A `set<T>` *SHALL* be encoded exactly as a `list<T>`.

A `map<K,V>` *SHALL* be encoded exactly as a `list<T>`

Future Expansion
----------------

Currently, there are no plans to support

* Multiplicity for tags that *MAY* appear more than once in a particular message 
* Floating-Lengths Packs (annotated `FLP`) although the `FLP` annotation is reserved for future use

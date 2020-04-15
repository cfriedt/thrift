#ifndef ST0601_GETTER_H_
#define ST0601_GETTER_H_

#include <unordered_map>
#include <unordered_set>

#include <thrift/protocol/St0601Tag.h>

#include "St0601.h"

namespace org {
namespace misb {

// can be overridden for custom
class St0601Getter {
public:

    using getter = void (*)(UasDataLinkLocalSet & msg /* out */);

    St0601Getter();
    virtual ~St0601Getter();

    virtual void getTags(const std::unordered_set<St0601Tag> & tags, UasDataLinkLocalSet & msg /* out */ ) const final;

protected:
    std::unordered_map<St0601Tag,St0601Getter::getter> getters;
};

}
}

#endif /* ST0601_GETTERS_H_ */

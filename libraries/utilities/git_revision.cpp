#include <stdint.h>
#include <graphene/utilities/git_revision.hpp>

#define GRAPHENE_GIT_REVISION_SHA "9b80bbb5bbc5f08faad06976e98db867b36e700b"
#define GRAPHENE_GIT_REVISION_UNIX_TIMESTAMP 1494822758
#define GRAPHENE_GIT_REVISION_DESCRIPTION "2.0.160121-136-g9b80bbb"

namespace graphene { namespace utilities {

const char* const git_revision_sha = GRAPHENE_GIT_REVISION_SHA;
const uint32_t git_revision_unix_timestamp = GRAPHENE_GIT_REVISION_UNIX_TIMESTAMP;
const char* const git_revision_description = GRAPHENE_GIT_REVISION_DESCRIPTION;

} } // end namespace graphene::utilities

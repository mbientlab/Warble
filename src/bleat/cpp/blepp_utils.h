/**
 * @copyright MbientLab License
 */
#pragma once

#ifdef API_BLEPP

#include <string>
#include "blepp/blestatemachine.h"

std::string uuid_to_string(const BLEPP::UUID& uuid);

#endif
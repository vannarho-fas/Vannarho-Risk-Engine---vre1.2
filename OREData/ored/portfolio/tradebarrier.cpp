/*
 Copyright (C) 2021 Quaternion Risk Management Ltd
 Copyright (C) 2024 Growth Mindset Pty Ltd
 All rights reserved.

 This file is part of VRE, a free-software/open-source library
 for transparent pricing and risk analysis

 VRE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.


 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

#include <ored/portfolio/tradebarrier.hpp>

namespace ore {
namespace data {


void TradeBarrier::fromXML(XMLNode* node) { TradeMonetary::fromXMLNode(node); }

XMLNode* TradeBarrier::toXML(XMLDocument& doc) {
    XMLNode* node = doc.allocNode("LevelData");
    TradeMonetary::toXMLNode(doc, node);
    return node;
}

} // namespace data
} // namespace ore
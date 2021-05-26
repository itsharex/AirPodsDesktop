//
// AirPodsDesktop - AirPods Desktop User Experience Enhancement Program.
// Copyright (C) 2021 SpriteOvO
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include "AppleCP.h"


namespace Core::AppleCP
{
    bool AirPods::IsValid(const std::vector<uint8_t> &data)
    {
        if (data.size() != sizeof(AirPods)) {
            return false;
        }

        constexpr uint8_t shouldRemainingLength =
            sizeof(AirPods) - (offsetof(Header, remainingLength) + sizeof(Header::remainingLength));

        Header *packet = (Header *)(data.data());
        if (packet->packetType != PacketType::ProximityPairing ||
            packet->remainingLength != shouldRemainingLength)
        {
            return false;
        }

        return true;
    }

    Core::AirPods::Side AirPods::GetBroadcastedSide() const
    {
        return broadcastFrom == 1 ? Core::AirPods::Side::Left : Core::AirPods::Side::Right;
    }

    bool AirPods::IsLeftBroadcasted() const
    {
        return GetBroadcastedSide() == Core::AirPods::Side::Left;
    }

    bool AirPods::IsRightBroadcasted() const
    {
        return GetBroadcastedSide() == Core::AirPods::Side::Right;
    }

    Core::AirPods::Model AirPods::GetModel() const
    {
        switch (modelId)
        {
        case 0x2002:
            return Core::AirPods::Model::AirPods_1;
        case 0x200F:
            return Core::AirPods::Model::AirPods_2;
        case 0x200E:
            return Core::AirPods::Model::AirPods_Pro;
        //case 0x2003:
        //    return Core::AirPods::Model::Powerbeats_3;
        //case 0x2005:
        //    return Core::AirPods::Model::Beats_X;
        //case 0x2006:
        //    return Core::AirPods::Model::Beats_Solo3;
        default:
            return Core::AirPods::Model::Unknown;
        }
    }

    Core::AirPods::Battery AirPods::GetLeftBattery() const
    {
        const auto val = (IsLeftBroadcasted() ? battery.curr : battery.anot);
        return (val >= 0 && val <= 10) ? val : Core::AirPods::Battery{std::nullopt};
    }

    Core::AirPods::Battery AirPods::GetRightBattery() const
    {
        const auto val = (IsRightBroadcasted() ? battery.curr : battery.anot);
        return (val >= 0 && val <= 10) ? val : Core::AirPods::Battery{std::nullopt};
    }

    Core::AirPods::Battery AirPods::GetCaseBattery() const
    {
        const auto val = battery.caseBox;
        return (val >= 0 && val <= 10) ? val : Core::AirPods::Battery{std::nullopt};
    }

    bool AirPods::IsLeftCharging() const
    {
        return (IsLeftBroadcasted() ? battery.currCharging : battery.anotCharging) != 0;
    }

    bool AirPods::IsRightCharging() const
    {
        return (IsRightBroadcasted() ? battery.currCharging : battery.anotCharging) != 0;
    }

    bool AirPods::IsBothPodsInCase() const
    {
        return bothInCase;
    }

    bool AirPods::IsLidOpened() const
    {
        return lidState == 1 || lidState == 2 || lidState == 3 || lidState == 4 ||
            lidState == 5 || lidState == 6 || lidState == 7 || lidState == 0;
    }

    bool AirPods::IsCaseCharging() const
    {
        return battery.caseCharging;
    }

    bool AirPods::IsLeftInEar() const
    {
        // If it's charging, the "ear" will be set in one of the multiple devices, idk why..
        // so we need to filter it
        //     vvvvvvvvvvvvvvvvvvvv
        return !IsLeftCharging() && (IsLeftBroadcasted() ? currInEar : anotInEar);
    }

    bool AirPods::IsRightInEar() const
    {
        return !IsRightCharging() && (IsRightBroadcasted() ? currInEar : anotInEar);
    }

} // namespace Core::AppleCP
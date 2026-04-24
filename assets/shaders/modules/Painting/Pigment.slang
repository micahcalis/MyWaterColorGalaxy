#include "Core/BeerMacros.slang"

import Core.BeerCore;

module Pigment;

namespace Painting
{
    public enum Pigment : int
    {
        PhthaloBlue = 0, 
        QuinacridoneMagenta = 1,
        HansaYellow = 2, 
        BurntUmber = 3  
    };

    public struct PigmentLocation
    {
        public int Layer;
        public int ChannelIndex;

        public __init(Pigment pigment)
        {
            Layer = (int)pigment / 4;
            ChannelIndex = (int)pigment % 4;
        }
    }
}
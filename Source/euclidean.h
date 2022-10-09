/*
  ==============================================================================

    euclidean.h
    Created: 9 Oct 2022 9:05:42pm
    Author:  Andreas Sandersen

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class euclidean
{
    
public:
    euclidean();
    ~euclidean() override;
    
    //Euclidean algorithm
    std::string euclidean(int pulses, int steps)
    {
        //We can only have as many beats as we have steps (0 <= beats <= steps)
        if (pulses > steps)
            pulses = steps;

        //Each iteration is a process of pairing strings X and Y and the remainder from the pairings
        //X will hold the "dominant" pair (the pair that there are more of)
        std::string x = "1";
        int x_amount = pulses;

        std::string y = "0";
        int y_amount = steps - pulses;

        do
        {
            //Placeholder variables
            int x_temp = x_amount;
            int y_temp = y_amount;
            std::string y_copy = y;

            //Check which is the dominant pair
            if (x_temp >= y_temp)
            {
                //Set the new number of pairs for X and Y
                x_amount = y_temp;
                y_amount = x_temp - y_temp;

                //The previous dominant pair becomes the new non dominant pair
                y = x;
            }
            else
            {
                x_amount = x_temp;
                y_amount = y_temp - x_temp;
            }

            //Create the new dominant pair by combining the previous pairs
            x = x + y_copy;
        } while (x_amount > 1 && y_amount > 1);//iterate as long as the non dominant pair can be paired (if there is 1 Y left, all we can do is pair it with however many Xs are left, so we're done)

                                               //By this point, we have strings X and Y formed through a series of pairings of the initial strings "1" and "0"
                                               //X is the final dominant pair and Y is the second to last dominant pair
        std::string rhythm;
        for (int i = 1; i <= x_amount; i++)
            rhythm += x;
        for (int i = 1; i <= y_amount; i++)
            rhythm += y;
        return rhythm;
    }
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Arp)
};

// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fastrtps/rtps/common/CacheChange.h>

#include <climits>
#include <vector>
#include <gtest/gtest.h>

using namespace eprosima::fastrtps::rtps;

struct FragmentTestStep
{
    std::string fail_description;

    struct __Input
    {
        uint32_t initial_fragment;
        uint16_t num_fragments;
    } input;

    struct __Check
    {
        bool missing_fragments[10];
    } check;

    void do_test(CacheChange_t& uut) const
    {
        if (input.num_fragments > 0)
        {
            uut.received_fragments(input.initial_fragment, input.num_fragments);
        }

        FragmentNumberSet_t fns;
        uut.get_missing_fragments(fns);

        for (FragmentNumber_t i = 1; i <= 10; i++)
        {
            EXPECT_EQ(fns.is_set(i), check.missing_fragments[i - 1]) 
                << "  on: " << fail_description << std::endl << "  index: " << i-1;
        }
    }
};

/*!
 * @fn TEST(CacheChange, FragmentManagement)
 * @brief This test checks the fragment management behavior of CacheChange_t.
 */
TEST(CacheChange, FragmentManagement)
{
    const FragmentTestStep test_steps[] =
    {
        {
            "initial state",
            {0, 0},
            {true, true, true, true, true, true, true, true, true, true}
        },
        {
            "received out-of-bounds (10)",
            {10, 1},
            {true, true, true, true, true, true, true, true, true, true}
        },
        {
            "received (1)",
            {1, 1},
            {true, false, true, true, true, true, true, true, true, true}
        },
        {
            "received (1) again",
            {1, 1},
            {true, false, true, true, true, true, true, true, true, true}
        },
        {
            "received (1, 2)",
            {1, 2},
            {true, false, false, true, true, true, true, true, true, true}
        },
        {
            "received (0)",
            {0, 1},
            {false, false, false, true, true, true, true, true, true, true}
        },
        {
            "received (0) again",
            {0, 1},
            {false, false, false, true, true, true, true, true, true, true}
        },
        {
            "received (8)",
            {8, 1},
            {false, false, false, true, true, true, true, true, false, true}
        },
        {
            "received (7, 8, 9, 10)",
            {7, 4},
            {false, false, false, true, true, true, true, false, false, false}
        },
        {
            "received (6)",
            {6, 1},
            {false, false, false, true, true, true, false, false, false, false}
        },
        {
            "received (3, 4, 5)",
            {3, 3},
            {false, false, false, false, false, false, false, false, false, false}
        }
    };

    CacheChange_t uut(90);
    uut.serializedPayload.length = 90;
    
    uut.setFragmentSize(9, true);
    for (const FragmentTestStep& step : test_steps)
    {
        step.do_test(uut);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

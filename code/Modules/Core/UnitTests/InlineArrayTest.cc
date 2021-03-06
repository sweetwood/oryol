//------------------------------------------------------------------------------
//  InlineArrayTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <algorithm>
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/InlineArray.h"
#include "Core/String/String.h"
#include "Core/Containers/KeyValuePair.h"

using namespace Oryol;

struct myClass {
    myClass() { };
    myClass(int v): val(v) { };
    ~myClass() { val = 0; };
    myClass(const myClass& rhs) { val = rhs.val; };
    myClass(myClass&& rhs) { val = rhs.val; rhs.val = 0; };
    void operator=(const myClass& rhs) { val = rhs.val; };
    void operator=(myClass&& rhs) { val = rhs.val; rhs.val = 0; };

    int val = 1;
};

TEST(InlineArrayTest) {

    InlineArray<myClass, 32> arr0;
    CHECK(arr0.Empty());
    CHECK(arr0.Size() == 0);
    CHECK(arr0.Capacity() == 32);
    CHECK(arr0.Spare() == 32);

    // populate array 
    myClass one(1), two(2), three(3);
    arr0.Add(one);
    CHECK(!arr0.Empty());
    CHECK(arr0.Size() == 1);
    CHECK(arr0.Spare() == 31);
    CHECK(arr0[0].val == 1);
    arr0.Add(std::move(two));
    CHECK(two.val == 0);
    CHECK(arr0.Size() == 2);
    CHECK(arr0.Spare() == 30);
    CHECK(arr0[1].val == 2);
    two = myClass(2);
    arr0.Add({ one, two, three });
    CHECK(arr0.Size() == 5);
    CHECK(arr0[0].val == 1);
    CHECK(arr0[1].val == 2);
    CHECK(arr0[2].val == 1);
    CHECK(arr0[3].val == 2);
    CHECK(arr0[4].val == 3);

    // iterate over array
    int values[] = { 1, 2, 1, 2, 3 };
    int i = 0;
    for (auto& v : arr0) {
        CHECK(v.val == values[i++]);
    }

    // copy construction
    InlineArray<myClass, 32> arr1(arr0);
    CHECK(arr1.Size() == arr0.Size());
    CHECK(arr1.Capacity() == arr0.Capacity());
    for (int i = 0; i < arr0.Size(); i++) {
        CHECK(arr0[i].val == arr1[i].val);
    }

    // move construction
    InlineArray<myClass, 32> arr2(std::move(arr0));
    CHECK(arr2.Size() == 5);
    CHECK(arr0.Size() == 0);
    // EVIL CODE to test whether moved elements have been default-initialized,
    // don't do this in real code
    CHECK(arr0.begin()->val == 0);
    // check if movement was successful
    for (int i = 0; i < arr2.Size(); i++) {
        CHECK(arr2[i].val == arr1[i].val);
    }

    // copy-assignment
    arr0 = { 5, 6, 7, 8, 9, 10 };
    CHECK(arr0.Size() == 6);
    arr1 = arr0;
    CHECK(arr0.Size() == 6);
    CHECK(arr1.Size() == 6);
    for (int i = 0; i < 6; i++) {
        CHECK(arr0[i].val == arr1[i].val);
    }
    // move-assignment 
    arr2 = std::move(arr0);
    CHECK(arr0.Empty());
    CHECK(arr2.Size() == 6);
    for (int i = 0; i < 6; i++) {
        CHECK(arr2[i].val == arr1[i].val);
    }

    // check if copying and moving a smaller array into a bigger
    // array works (the left-over items in target array must 
    // be default-initialized afterwards)
    arr0 = { 1, 2, 3 };
    arr1 = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
    arr2 = arr1;
    arr1 = arr0;
    CHECK(arr1[0].val == 1);
    CHECK(arr1[1].val == 2);
    CHECK(arr1[2].val == 3);
    // EVIL, DON'T DO THIS in actual code
    for (int i = 3; i < 10; i++) {
        CHECK(arr1.begin()[i].val == 1);
    }

    arr2 = std::move(arr0);
    CHECK(arr0.Empty());
    CHECK(arr2[0].val == 1);
    CHECK(arr2[1].val == 2);
    CHECK(arr2[2].val == 3);
    // EVIL, DON'T DO THIS in actual code
    for (int i = 3; i < 10; i++) {
        CHECK(arr2.begin()[i].val == 1);
    }
}


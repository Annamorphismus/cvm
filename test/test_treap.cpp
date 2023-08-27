#include <cvm/treap.hpp>
#include <gtest/gtest.h>

using cvm::Treap;

TEST(TreapBasic, InsertAndSize)
{
    Treap<int> treap;
    treap.insert(5, 100); // higher priority
    EXPECT_EQ(treap.size(), 1);
    treap.insert(2, 50); // lower priority
    treap.insert(8, 75); // medium priority
    EXPECT_EQ(treap.size(), 3);
}

TEST(TreapTests, DeleteElem)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(2, 50);
    treap.insert(8, 75);
    treap.delete_elem(2);
    EXPECT_EQ(treap.size(), 2);
    treap.delete_elem(5);
    EXPECT_EQ(treap.size(), 1);
}

TEST(TreapTests, MultipleInsertsWithPriorities)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(3, 90);
    treap.insert(8, 95);
    treap.insert(1, 80);
    treap.insert(7, 85);

    EXPECT_EQ(treap.size(), 5);
}

TEST(TreapTests, DeleteFromMultipleWithPriorities)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(3, 90);
    treap.insert(8, 95);

    treap.delete_elem(5);
    EXPECT_EQ(treap.size(), 2);
}



TEST(TreapTests, BasicInsert)
{
    Treap<int> treap;
    treap.insert(5);
    EXPECT_EQ(treap.size(), 1);
}

TEST(TreapTests, MultipleInserts)
{
    Treap<int> treap;
    treap.insert(5);
    treap.insert(3);
    treap.insert(8);
    treap.insert(1);
    treap.insert(7);

    EXPECT_EQ(treap.size(), 5);
}

// 3. Basic Delete
TEST(TreapTests, BasicDelete)
{
    Treap<int> treap;
    treap.insert(5);
    treap.delete_elem(5);
    EXPECT_EQ(treap.size(), 0);
}

TEST(TreapTests, Pop)
{
    Treap<int> treap;
    treap.insert(5, 100); // This will be the root due to highest priority
    treap.insert(2, 50);  // This will be the left child of the root
    treap.insert(8, 75);  // This will be the right child of the root

    auto val = treap.pop();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value().first, 5); // Root is popped
    EXPECT_EQ(treap.size(), 2);
    val = treap.pop();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value().first, 8); // Higher priority than 2, so it's popped next
    EXPECT_EQ(treap.size(), 1);
}

TEST(TreapTests, DeterministicPopRoot)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(3, 90);
    treap.insert(8, 95);

    auto rootValue = treap.pop();
    EXPECT_TRUE(rootValue.has_value());
    EXPECT_EQ(rootValue->first, 5); // This should always pop 5 as it has the highest priority
    EXPECT_EQ(treap.size(), 2);
}


TEST(TreapTests, CopyConstructor)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(2, 50);

    Treap<int> copiedTreap = treap;
    EXPECT_EQ(copiedTreap.size(), 2);

    copiedTreap.insert(8, 75);
    EXPECT_EQ(copiedTreap.size(), 3);

    EXPECT_EQ(treap.size(), 2); // Ensure the original treap was not modified
}

TEST(TreapTests, MoveConstructor)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(2, 50);
    Treap<int> movedTreap = std::move(treap);
    EXPECT_EQ(movedTreap.size(), 2);
    EXPECT_EQ(treap.size(), 0); // Ensure the original treap was emptied
}

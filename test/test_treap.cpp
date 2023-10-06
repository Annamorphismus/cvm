#include <cvm/treap.hpp>
#include <gtest/gtest.h>

using cvm::Treap;


// Ein Test für grundlegende Treap-Operationen wie Einfügen und Größe abfragen.
TEST(TreapBasic, InsertAndSize)
{
    // Erzeuge ein neues Treap-Objekt.
    Treap<int> treap;

    // Füge das Element 5 mit einer hohen Priorität von 100 ein.
    treap.insert(5, 100);

    // Überprüfe, ob die Größe des Treaps jetzt 1 ist.
    EXPECT_EQ(treap.size(), 1);

    // Füge weitere Elemente mit verschiedenen Prioritäten hinzu.
    treap.insert(2, 50); // lower priority
    treap.insert(8, 75); // medium priority

    // Überprüfe, ob die Größe des Treaps jetzt 3 ist.
    EXPECT_EQ(treap.size(), 3);
}

// Ein Test für das Löschen von Elementen aus dem Treap.
TEST(TreapTests, DeleteElem)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(2, 50);
    treap.insert(8, 75);

    // Lösche das Element 2.
    treap.delete_elem(2);

    // Überprüfe, ob die Größe des Treaps jetzt 2 ist.
    EXPECT_EQ(treap.size(), 2);

    // Lösche das Element 5.
    treap.delete_elem(5);

    // Überprüfe, ob die Größe des Treaps jetzt 1 ist.
    EXPECT_EQ(treap.size(), 1);
}


// Testet das Einfügen mehrerer Elemente mit unterschiedlichen Prioritäten in den Treap.
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


// Überprüft das Löschen von Elementen aus einem Treap, der bereits mehrere Elemente mit unterschiedlichen Prioritäten enthält.
TEST(TreapTests, DeleteFromMultipleWithPriorities)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(3, 90);
    treap.insert(8, 95);

    treap.delete_elem(5);
    EXPECT_EQ(treap.size(), 2);
}


// Testet ein einfaches Einfügen ohne Priorität in den Treap.
TEST(TreapTests, BasicInsert)
{
    Treap<int> treap;
    treap.insert(5);
    EXPECT_EQ(treap.size(), 1);
}

// Testet das Einfügen mehrerer Elemente ohne spezifische Priorität in den Treap.
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

// Überprüft die grundlegende Löschfunktion in einem Treap.
TEST(TreapTests, BasicDelete)
{
    Treap<int> treap;
    treap.insert(5);
    treap.delete_elem(5);
    EXPECT_EQ(treap.size(), 0);
}


// Testet die `pop`-Funktion, die das Element mit der höchsten Priorität entfernt und zurückgibt.
TEST(TreapTests, Pop)
{
    Treap<int> treap;
    treap.insert(5, 100); // Dies wird aufgrund der höchsten Priorität die Wurzel sein
    treap.insert(2, 50);  // Dies wird das linke Kind der Wurzel sein
    treap.insert(8, 75);  // Dies wird das rechte Kind der Wurzel sein

    auto val = treap.pop();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value().first, 5); // Die Wurzel wird entfernt
    EXPECT_EQ(treap.size(), 2);
    val = treap.pop();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value().first, 8); // Hat eine höhere Priorität als 2, wird also als nächstes entfernt
    EXPECT_EQ(treap.size(), 1);
}


// Überprüft, ob die `pop`-Funktion immer den Knoten mit der höchsten Priorität entfernt.
TEST(TreapTests, DeterministicPopRoot)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(3, 90);
    treap.insert(8, 95);

    auto rootValue = treap.pop();
    EXPECT_TRUE(rootValue.has_value());
    EXPECT_EQ(rootValue->first, 5); // Dies sollte immer 5 entfernen, da es die höchste Priorität hat
    EXPECT_EQ(treap.size(), 2);
}

// Ein Test für den Kopierkonstruktor des Treaps.
TEST(TreapTests, CopyConstructor)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(2, 50);

    // Erzeuge einen neuen Treap durch Kopieren des ursprünglichen Treaps.
    Treap<int> copiedTreap = treap;

    // Überprüfe, ob der kopierte Treap die gleiche Größe wie das Original hat.
    EXPECT_EQ(copiedTreap.size(), 2);
    // This should always pop 5 as it has the highest priority
    // Füge ein neues Element zum kopierten Treap hinzu.
    copiedTreap.insert(8, 75);

    // Überprüfe, ob die Größe des kopierten Treaps jetzt 3 ist.
    EXPECT_EQ(copiedTreap.size(), 3);

    // Stelle sicher, dass das ursprüngliche Treap-Objekt unverändert bleibt.
    EXPECT_EQ(treap.size(), 2);
}


// Ein Test für den Move-Konstruktor des Treaps.
TEST(TreapTests, MoveConstructor)
{
    Treap<int> treap;
    treap.insert(5, 100);
    treap.insert(2, 50);

    // Erzeuge einen neuen Treap durch Verschieben des ursprünglichen Treaps.
    Treap<int> movedTreap = std::move(treap);

    // Überprüfe, ob der verschobene Treap die Größe des Originals hat.
    EXPECT_EQ(movedTreap.size(), 2);

    // Stelle sicher, dass das ursprüngliche Treap jetzt leer ist.
    EXPECT_EQ(treap.size(), 0);
}

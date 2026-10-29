#include <gtest/gtest.h>
#include "Assets/AssetManager.hpp"
#include "Assets/Mesh.hpp"

class TestAsset : public Bonsai::IAssetType
{
public:
    explicit TestAsset(int foo)
        : foo(foo) {}

    int foo = 0;
};

TEST(AssetManagerTests, TestCreateAsset)
{
    Bonsai::AssetManager assetManager{};
    Bonsai::AssetHandle<TestAsset> testAsset = assetManager.Create<TestAsset>(42);
    ASSERT_TRUE(testAsset.IsValid());

    auto testAssetPtr = testAsset.Get();
    ASSERT_NE(testAssetPtr, nullptr);
    ASSERT_EQ(testAssetPtr->foo, 42);
}

TEST(AssetManagerTests, TestCreateMultipleAssets)
{
    Bonsai::AssetManager assetManager{};
    Bonsai::AssetHandle<TestAsset> testAsset32 = assetManager.Create<TestAsset>(32);
    Bonsai::AssetHandle<TestAsset> testAsset64 = assetManager.Create<TestAsset>(64);
    ASSERT_TRUE(testAsset32.IsValid());
    ASSERT_TRUE(testAsset64.IsValid());
    ASSERT_NE(testAsset32.Get(), testAsset64.Get());

    ASSERT_NE(testAsset32.Get(), nullptr);
    ASSERT_EQ(testAsset32.Get()->foo, 32);

    ASSERT_NE(testAsset64.Get(), nullptr);
    ASSERT_EQ(testAsset64.Get()->foo, 64);
}

TEST(AssetManagerTests, TestCreateThenRemoveAsset)
{
    Bonsai::AssetManager assetManager{};
    Bonsai::AssetHandle<TestAsset> testAsset = assetManager.Create<TestAsset>(42);
    ASSERT_TRUE(testAsset.IsValid());
    ASSERT_NE(assetManager.GetAsset(testAsset), nullptr);

    assetManager.Remove(testAsset);
    ASSERT_FALSE(testAsset.IsValid());
    ASSERT_EQ(assetManager.GetAsset(testAsset), nullptr);
}

TEST(AssetManagerTests, TestTryFindInvalidAsset)
{
    Bonsai::AssetManager assetManager{};
    ASSERT_EQ(assetManager.GetAsset(Bonsai::AssetHandle<TestAsset>()), nullptr);
}

TEST(AssetManagerTests, TestLoadSameAssetTwice)
{
    Bonsai::AssetManager assetManager{};
    Bonsai::Filesystem::Path const assetPath = Bonsai::Filesystem::GetResourcePath("resources/assets/cube.obj");

    Bonsai::AssetHandle<Bonsai::Mesh> const meshHandle = assetManager.Load<Bonsai::Mesh>(assetPath);
    ASSERT_TRUE(meshHandle.IsValid());

    Bonsai::AssetHandle<Bonsai::Mesh> const secondMeshHandle = assetManager.Load<Bonsai::Mesh>(assetPath);
    ASSERT_TRUE(secondMeshHandle.IsValid());
    ASSERT_EQ(meshHandle, secondMeshHandle);
}

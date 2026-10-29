#include <gtest/gtest.h>
#include "Assets/AssetImporter.hpp"
#include "Assets/Mesh.hpp"

TEST(MeshTests, TestLoadMeshFromFile)
{
    std::shared_ptr<Bonsai::Mesh> const mesh = Bonsai::AssetImporter<Bonsai::Mesh>::Import(Bonsai::Filesystem::GetResourcePath("resources/assets/cube.obj"));
    ASSERT_NE(mesh, nullptr);

    ASSERT_EQ(mesh->GetVertices().size(), 36);
    ASSERT_EQ(mesh->GetIndices().size(), 36);
}

TEST(MeshTests, TestLoadMeshFromInvalidFilePath)
{
    std::shared_ptr<Bonsai::Mesh> const mesh = Bonsai::AssetImporter<Bonsai::Mesh>::Import(Bonsai::Filesystem::GetResourcePath("resources/assets/DOES_NOT_EXIST"));
    ASSERT_EQ(mesh, nullptr);
}

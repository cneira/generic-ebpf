#include <gtest/gtest.h>

extern "C" {
#include <stdint.h>
#include <errno.h>

#include <dev/ebpf/ebpf_map.h>
}

namespace {

class PercpuArrayMapDeleteTest : public ::testing::Test {
      protected:
	struct ebpf_map map;

	virtual void
	SetUp()
	{
		int error;
		uint32_t gkey = 50;
		uint32_t gval = 100;

		error =
		    ebpf_map_init(&map, EBPF_MAP_TYPE_PERCPU_ARRAY,
				  sizeof(uint32_t), sizeof(uint32_t), 100, 0);
		ASSERT_TRUE(!error);

		error = ebpf_map_update_elem_from_user(&map, &gkey, &gval, 0);
		ASSERT_TRUE(!error);
	}

	virtual void
	TearDown()
	{
		ebpf_map_deinit(&map, NULL);
	}
};

/* Delete always failes */
TEST_F(PercpuArrayMapDeleteTest, CorrectDelete)
{
	int error;
	uint32_t key = 50;

	error = ebpf_map_delete_elem_from_user(&map, &key);

	EXPECT_EQ(EINVAL, error);
}
} // namespace

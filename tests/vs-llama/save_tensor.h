#include <string.h>
#include <stdlib.h>
// Add these at the top of save_tensor.h
#include <stdint.h>     // For uint8_t, uint32_t, uint64_t, etc.
#include <stdbool.h>    // For bool type
#include <string.h>     // For strlen, strcpy, memcpy
#include <stdlib.h>     // For malloc, free
#include <stdio.h>      // For FILE, fopen, fwrite, etc.
#include <sys/stat.h>  // Add this include at the top

// Add this function before intermediary_tensor_from_model_core
size_t model_core_total_byte_size(const struct ggml_tensor* tensor) {
	if (!tensor) {
		return 0;
	}

	// Use ggml's built-in function to calculate tensor byte size
	return ggml_nbytes(tensor);
}

// Constants (replacing static constexpr)
#define TENSOR_LEN_SIZE sizeof(uint64_t)
#define NAME_LEN_SIZE sizeof(uint64_t)
#define DIMS_SIZE (sizeof(size_t) * 4)
#define TYPE_SIZE sizeof(uint32_t)

// Pure C struct (no constructors, no std::vector)
typedef struct {
	uint8_t* data;
	size_t data_size;
	size_t data_capacity;
	size_t dims[4];
	char* name;
	size_t name_length;
	enum ggml_type type;
	enum ggml_op op;
} intermediary_tensor;

// Manual memory management functions
intermediary_tensor* intermediary_tensor_create(void) {
	intermediary_tensor* tensor = malloc(sizeof(intermediary_tensor));
	if (!tensor) {
		return NULL;
	}

	tensor->data		  = NULL;
	tensor->data_size	  = 0;
	tensor->data_capacity = 0;
	memset(tensor->dims, 0, sizeof(tensor->dims));
	tensor->name		= NULL;
	tensor->name_length = 0;
	tensor->type		= 0;
	tensor->op			= 0;

	return tensor;
}

void intermediary_tensor_destroy(intermediary_tensor* tensor) {
	if (!tensor) {
		return;
	}

	if (tensor->data) {
		free(tensor->data);
	}
	if (tensor->name) {
		free(tensor->name);
	}
	free(tensor);
}

// Resize data array (replacing std::vector::resize)
bool intermediary_tensor_resize_data(intermediary_tensor* tensor, size_t new_size) {
	if (!tensor) {
		return false;
	}

	if (new_size > tensor->data_capacity) {
		size_t new_capacity = new_size * 2;// Growth strategy
		uint8_t* new_data	= realloc(tensor->data, new_capacity);
		if (!new_data) {
			return false;
		}

		tensor->data		  = new_data;
		tensor->data_capacity = new_capacity;
	}

	tensor->data_size = new_size;
	return true;
}

// Set name (replacing std::string assignment)
bool intermediary_tensor_set_name(intermediary_tensor* tensor, const char* name) {
	if (!tensor) {
		return false;
	}

	if (tensor->name) {
		free(tensor->name);
	}

	if (!name) {
		tensor->name		= NULL;
		tensor->name_length = 0;
		return true;
	}

	size_t len	 = strlen(name);
	tensor->name = malloc(len + 1);
	if (!tensor->name) {
		return false;
	}

	strcpy(tensor->name, name);
	tensor->name_length = len;
	return true;
}

// Constructor equivalent from model_core
intermediary_tensor* intermediary_tensor_from_model_core(const struct ggml_tensor* other) {
	if (!other) {
		return NULL;
	}

	intermediary_tensor* tensor = intermediary_tensor_create();
	if (!tensor) {
		return NULL;
	}

	// Copy data
	size_t nbytes = model_core_total_byte_size(other);// Assume this function exists
	if (!intermediary_tensor_resize_data(tensor, nbytes)) {
		intermediary_tensor_destroy(tensor);
		return NULL;
	}

	memcpy(tensor->data, other->data, nbytes);

	// Copy dimensions
	for (size_t x = 0; x < 4; ++x) {
		tensor->dims[x] = other->ne[x];
	}

	// Copy type and name
	tensor->type = other->type;
	if (!intermediary_tensor_set_name(tensor, other->name)) {
		intermediary_tensor_destroy(tensor);
		return NULL;
	}

	return tensor;
}

// Comparison function (replacing operator==)
bool intermediary_tensor_equals(const intermediary_tensor* a, const intermediary_tensor* b) {
	if (!a || !b) {
		return false;
	}

	// Compare basic fields
	if (a->type != b->type || a->data_size != b->data_size || a->name_length != b->name_length) {
		return false;
	}

	// Compare dimensions
	for (size_t i = 0; i < 4; ++i) {
		if (a->dims[i] != b->dims[i]) {
			return false;
		}
	}

	// Compare names
	if (a->name_length > 0) {
		if (strcmp(a->name, b->name) != 0) {
			return false;
		}
	}

	// Compare data
	if (a->data_size > 0) {
		if (memcmp(a->data, b->data, a->data_size) != 0) {
			return false;
		}
	}

	return true;
}

// Serialization function (replacing std::string return)
typedef struct {
	uint8_t* data;
	size_t size;
} serialized_tensor;

serialized_tensor serialize_tensor(const intermediary_tensor* tensor) {
	serialized_tensor result = { NULL, 0 };

	if (!tensor) {
		return result;
	}

	// Calculate total size
	size_t total_size = TYPE_SIZE + NAME_LEN_SIZE + TENSOR_LEN_SIZE + DIMS_SIZE + tensor->data_size + tensor->name_length;

	// Allocate buffer
	uint8_t* buffer = malloc(total_size);
	if (!buffer) {
		return result;
	}

	size_t current_offset = 0;

	// Copy type
	memcpy(buffer + current_offset, &tensor->type, TYPE_SIZE);
	current_offset += TYPE_SIZE;

	// Copy dimensions
	memcpy(buffer + current_offset, tensor->dims, DIMS_SIZE);
	current_offset += DIMS_SIZE;

	// Copy name length and name
	uint64_t name_length = tensor->name_length;
	memcpy(buffer + current_offset, &name_length, NAME_LEN_SIZE);
	current_offset += NAME_LEN_SIZE;

	if (tensor->name_length > 0) {
		memcpy(buffer + current_offset, tensor->name, tensor->name_length);
		current_offset += tensor->name_length;
	}

	// Copy tensor data length and data
	uint64_t tensor_length = tensor->data_size;
	memcpy(buffer + current_offset, &tensor_length, TENSOR_LEN_SIZE);
	current_offset += TENSOR_LEN_SIZE;

	if (tensor->data_size > 0) {
		memcpy(buffer + current_offset, tensor->data, tensor->data_size);
	}

	result.data = buffer;
	result.size = total_size;
	return result;
}

// Free serialized tensor
void serialized_tensor_free(serialized_tensor* tensor) {
	if (tensor && tensor->data) {
		free(tensor->data);
		tensor->data = NULL;
		tensor->size = 0;
	}
}

typedef enum {
	FILE_SAVER_SUCCESS = 0,
	FILE_SAVER_ERROR_NULL_PARAMS,
	FILE_SAVER_ERROR_ZERO_SIZE,
	FILE_SAVER_ERROR_OPEN_FAILED,
	FILE_SAVER_ERROR_WRITE_FAILED,
	FILE_SAVER_ERROR_CLOSE_FAILED
} file_saver_error;

file_saver_error file_saver_c_detailed(const char* filename, const void* data, size_t data_size) {
	// Parameter validation
	if (!filename || !data) {
		return FILE_SAVER_ERROR_NULL_PARAMS;
	}

	if (data_size == 0) {
		return FILE_SAVER_ERROR_ZERO_SIZE;
	}

	// Open file for binary writing
	FILE* file = fopen(filename, "wb");
	if (!file) {
		return FILE_SAVER_ERROR_OPEN_FAILED;
	}

	// Write data in chunks (for large files)
	const uint8_t* byte_data = ( const uint8_t* )data;
	size_t total_written	 = 0;

	while (total_written < data_size) {
		size_t chunk_size = data_size - total_written;
		if (chunk_size > 1024 * 1024) {// 1MB chunks
			chunk_size = 1024 * 1024;
		}

		size_t written = fwrite(byte_data + total_written, 1, chunk_size, file);
		if (written != chunk_size) {
			fclose(file);
			return FILE_SAVER_ERROR_WRITE_FAILED;
		}

		total_written += written;
	}

	// Flush and close
	if (fflush(file) != 0) {
		fclose(file);
		return FILE_SAVER_ERROR_WRITE_FAILED;
	}

	if (fclose(file) != 0) {
		return FILE_SAVER_ERROR_CLOSE_FAILED;
	}

	return FILE_SAVER_SUCCESS;
}

// Simple bool wrapper
bool file_saver_c(const char* filename, const void* data, size_t data_size) {
	return file_saver_c_detailed(filename, data, data_size) == FILE_SAVER_SUCCESS;
}

bool file_exists(const char* filename) {
	if (!filename) {
		return false;
	}

	struct stat buffer;
	return (stat(filename, &buffer) == 0);
}

// Modified save_tensor_c function
bool save_tensor_c(const struct ggml_tensor* core) {
	if (!core) {
		return false;
	}

	// Create filename first to check if it exists
	size_t filename_len = strlen(core->name) + strlen(".safetensor") + 1;
	char* filename		= malloc(filename_len);
	if (!filename) {
		return false;
	}

	strcpy(filename, core->name);
	strcat(filename, ".safetensor");

	// CHECK IF FILE ALREADY EXISTS - SKIP IF IT DOES!
	if (file_exists(filename)) {
		free(filename);
		return true;// Return true since we "successfully" didn't overwrite
	}

	// Continue with normal tensor creation only if file doesn't exist
	intermediary_tensor* save_tensor = intermediary_tensor_from_model_core(core);
	if (!save_tensor) {
		free(filename);
		return false;
	}

	serialized_tensor serialized = serialize_tensor(save_tensor);
	if (!serialized.data) {
		intermediary_tensor_destroy(save_tensor);
		free(filename);
		return false;
	}

	// Save file
	bool success = file_saver_c(filename, serialized.data, serialized.size);

	// Cleanup
	free(filename);
	serialized_tensor_free(&serialized);
	intermediary_tensor_destroy(save_tensor);

	return success;
}
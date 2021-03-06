/*-
 * SPDX-License-Identifier: Apache License 2.0
 *
 * Copyright 2017-2018 Yutaro Hayakawa
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*-
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (c) 2018 Yutaro Hayakawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <dev/ebpf/ebpf_platform.h>
#include <dev/ebpf/ebpf_map.h>
#include <sys/ebpf.h>

void *
ebpf_malloc(size_t size)
{
	return malloc(size);
}

void *
ebpf_calloc(size_t number, size_t size)
{
	return calloc(number, size);
}

void *
ebpf_exalloc(size_t size)
{
	void *ret = NULL;

	ret = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC,
		   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ret == MAP_FAILED) {
		fprintf(stderr, "mmap in ebpf_exalloc failed\n");
		return NULL;
	}

	return ret;
}

void
ebpf_exfree(void *mem, size_t size)
{
	munmap(mem, size);
}

void
ebpf_free(void *mem)
{
	free(mem);
}

int
ebpf_error(const char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = vfprintf(stderr, fmt, ap);
	va_end(ap);

	return ret;
}

void
ebpf_assert(bool expr)
{
	assert(expr);
}

uint16_t
ebpf_ncpus(void)
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}

uint16_t
ebpf_curcpu(void)
{
	return 0; // This makes no sense. Just for testing.
}

long
ebpf_getpagesize(void)
{
	return sysconf(_SC_PAGESIZE);
}

void
ebpf_rw_init(ebpf_rwlock_t *rw, char *name)
{
	int error = pthread_rwlock_init(rw, NULL);
	assert(!error);
}

void
ebpf_rw_rlock(ebpf_rwlock_t *rw)
{
	int error = pthread_rwlock_rdlock(rw);
	assert(!error);
}

void
ebpf_rw_runlock(ebpf_rwlock_t *rw)
{
	int error = pthread_rwlock_unlock(rw);
	assert(!error);
}

void
ebpf_rw_wlock(ebpf_rwlock_t *rw)
{
	int error = pthread_rwlock_wrlock(rw);
	assert(!error);
}

void
ebpf_rw_wunlock(ebpf_rwlock_t *rw)
{
	int error = pthread_rwlock_unlock(rw);
	assert(!error);
}

void
ebpf_rw_destroy(ebpf_rwlock_t *rw)
{
	int error = pthread_rwlock_destroy(rw);
	assert(!error);
}

/*
 * FIXME Below epoch and refcount~ things are just a stub and doesn't work
 * correctly. In future version, replace them to correct one.
 */

void
ebpf_epoch_enter(void)
{
	return;
}

void
ebpf_epoch_exit(void)
{
	return;
}

void
ebpf_epoch_call(ebpf_epoch_context_t *ctx,
		void (*callback)(ebpf_epoch_context_t *))
{
	return;
}

void
ebpf_epoch_wait(void)
{
	return;
}

void
ebpf_refcount_init(volatile uint32_t *count, uint32_t val)
{
	*count = val;
}

void
ebpf_refcount_acquire(volatile uint32_t *count)
{
	*count++;
}

int
ebpf_refcount_release(volatile uint32_t *count)
{
	*count--;
	if (count == 0) {
		return 1;
	}
	return 0;
}

void
ebpf_mtx_init(ebpf_mtx_t *mutex, const char *name)
{
	int error = pthread_mutex_init(mutex, NULL);
	assert(!error);
}

void
ebpf_mtx_lock(ebpf_mtx_t *mutex)
{
	int error = pthread_mutex_lock(mutex);
	assert(!error);
}

void
ebpf_mtx_unlock(ebpf_mtx_t *mutex)
{
	int error = pthread_mutex_unlock(mutex);
	assert(!error);
}

void
ebpf_mtx_destroy(ebpf_mtx_t *mutex)
{
	int error = pthread_mutex_destroy(mutex);
	assert(!error);
}

void
ebpf_init_map_types(void)
{
	for (uint16_t i = 0; i < __EBPF_MAP_TYPE_MAX; i++) {
		ebpf_register_map_type(i, &bad_map_ops);
	}

	ebpf_register_map_type(EBPF_MAP_TYPE_ARRAY, &array_map_ops);
	ebpf_register_map_type(EBPF_MAP_TYPE_PERCPU_ARRAY,
			       &percpu_array_map_ops);
}

__attribute__((constructor)) void
ebpf_init(void)
{
	ebpf_init_map_types();
}

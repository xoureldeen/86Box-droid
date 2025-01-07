/*
 * Generic FIFO component, implemented as a circular buffer.
 *
 * Copyright (c) 2012 Peter A. G. Crosthwaite
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include <86box/86box.h>
#include <86box/fifo8.h>

void
fifo8_reset(Fifo8 *fifo)
{
    fifo->num  = 0;
    fifo->head = 0;
}

void
fifo8_create(Fifo8 *fifo, uint32_t capacity)
{
    fifo->data = (uint8_t *) calloc(1, capacity);
    fifo->capacity = capacity;
    fifo8_reset(fifo);
}

void
fifo8_destroy(Fifo8 *fifo)
{
    if (fifo->data) {
        free(fifo->data);
        fifo->data = NULL;
    }
}

void
fifo8_push(Fifo8 *fifo, uint8_t data)
{
    assert(fifo->num < fifo->capacity);
    fifo->data[(fifo->head + fifo->num) % fifo->capacity] = data;
    fifo->num++;
}

void
fifo8_push_all(Fifo8 *fifo, const uint8_t *data, uint32_t num)
{
    uint32_t start;
    uint32_t avail;

    assert((fifo->num + num) <= fifo->capacity);

    start = (fifo->head + fifo->num) % fifo->capacity;

    if (start + num <= fifo->capacity) {
        memcpy(&fifo->data[start], data, num);
    } else {
        avail = fifo->capacity - start;
        memcpy(&fifo->data[start], data, avail);
        memcpy(&fifo->data[0], &data[avail], num - avail);
    }

    fifo->num += num;
}

uint8_t
fifo8_pop(Fifo8 *fifo)
{
    uint8_t ret;

    assert(fifo->num > 0);
    ret = fifo->data[fifo->head++];
    fifo->head %= fifo->capacity;
    fifo->num--;
    return ret;
}

static const uint8_t
*fifo8_peekpop_buf(Fifo8 *fifo, uint32_t max, uint32_t *numptr, int do_pop)
{
    uint8_t *ret;
    uint32_t num;

    assert((max > 0) && (max <= fifo->num));
    num = MIN(fifo->capacity - fifo->head, max);
    ret = &fifo->data[fifo->head];

    if (do_pop) {
        fifo->head += num;
        fifo->head %= fifo->capacity;
        fifo->num -= num;
    }
    if (numptr)
        *numptr = num;

    return ret;
}

const uint8_t
*fifo8_peek_bufptr(Fifo8 *fifo, uint32_t max, uint32_t *numptr)
{
    return fifo8_peekpop_buf(fifo, max, numptr, 0);
}

const uint8_t
*fifo8_pop_bufptr(Fifo8 *fifo, uint32_t max, uint32_t *numptr)
{
    return fifo8_peekpop_buf(fifo, max, numptr, 1);
}

uint32_t
fifo8_pop_buf(Fifo8 *fifo, uint8_t *dest, uint32_t destlen)
{
    const uint8_t *buf;
    uint32_t n1, n2 = 0;
    uint32_t len;

    if (destlen == 0)
        return 0;

    len = destlen;
    buf = fifo8_pop_bufptr(fifo, len, &n1);
    if (dest)
        memcpy(dest, buf, n1);

    /* Add FIFO wraparound if needed */
    len -= n1;
    len = MIN(len, fifo8_num_used(fifo));
    if (len) {
        buf = fifo8_pop_bufptr(fifo, len, &n2);
        if (dest) {
            memcpy(&dest[n1], buf, n2);
        }
    }

    return n1 + n2;
}

void
fifo8_drop(Fifo8 *fifo, uint32_t len)
{
    len -= fifo8_pop_buf(fifo, NULL, len);
    assert(len == 0);
}

int
fifo8_is_empty(Fifo8 *fifo)
{
    return (fifo->num == 0);
}

int
fifo8_is_full(Fifo8 *fifo)
{
    return (fifo->num == fifo->capacity);
}

uint32_t
fifo8_num_free(Fifo8 *fifo)
{
    return fifo->capacity - fifo->num;
}

uint32_t
fifo8_num_used(Fifo8 *fifo)
{
    return fifo->num;
}
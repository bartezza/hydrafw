/*
 * HydraBus/HydraNFC
 *
 * Copyright (C) 2016 Andrea Bartezzaghi <andrea.bartezzaghi@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _HYDRANFC_SRIX_H_
#define _HYDRANFC_SRIX_H_

#include "common.h"
#include "mcu.h"

void hydranfc_srix_scan(t_hydra_console *con);

/* NOTE: pass NULL to filename to not dump to file, but just to console. */
void hydranfc_srix_dump(t_hydra_console *con, const char *filename);

void hydranfc_srix_clone(t_hydra_console *con, const char *filename);

void hydranfc_srix_fill_all(t_hydra_console *con, uint32_t byteValue);

#endif /* _HYDRANFC_SRIX_H_ */


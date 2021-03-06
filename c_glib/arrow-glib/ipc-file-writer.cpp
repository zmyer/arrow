/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <arrow/ipc/api.h>

#include <arrow-glib/array.hpp>
#include <arrow-glib/error.hpp>
#include <arrow-glib/record-batch.hpp>
#include <arrow-glib/schema.hpp>

#include <arrow-glib/io-output-stream.hpp>

#include <arrow-glib/ipc-stream-writer.hpp>
#include <arrow-glib/ipc-file-writer.hpp>

G_BEGIN_DECLS

/**
 * SECTION: ipc-file-writer
 * @short_description: File writer class
 *
 * #GArrowIPCFileWriter is a class for sending data by file based IPC.
 */

G_DEFINE_TYPE(GArrowIPCFileWriter,
              garrow_ipc_file_writer,
              GARROW_IPC_TYPE_STREAM_WRITER);

static void
garrow_ipc_file_writer_init(GArrowIPCFileWriter *object)
{
}

static void
garrow_ipc_file_writer_class_init(GArrowIPCFileWriterClass *klass)
{
}

/**
 * garrow_ipc_file_writer_open:
 * @sink: The output of the writer.
 * @schema: The schema of the writer.
 * @error: (nullable): Return locatipcn for a #GError or %NULL.
 *
 * Returns: (nullable) (transfer full): A newly opened
 *   #GArrowIPCFileWriter or %NULL on error.
 */
GArrowIPCFileWriter *
garrow_ipc_file_writer_open(GArrowIOOutputStream *sink,
                            GArrowSchema *schema,
                            GError **error)
{
  std::shared_ptr<arrow::ipc::FileWriter> arrow_file_writer;
  auto status =
    arrow::ipc::FileWriter::Open(garrow_io_output_stream_get_raw(sink).get(),
                                 garrow_schema_get_raw(schema),
                                 &arrow_file_writer);
  if (status.ok()) {
    return garrow_ipc_file_writer_new_raw(&arrow_file_writer);
  } else {
    garrow_error_set(error, status, "[ipc][file-writer][open]");
    return NULL;
  }
}

/**
 * garrow_ipc_file_writer_write_record_batch:
 * @file_writer: A #GArrowIPCFileWriter.
 * @record_batch: The record batch to be written.
 * @error: (nullable): Return locatipcn for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE if there was an error.
 */
gboolean
garrow_ipc_file_writer_write_record_batch(GArrowIPCFileWriter *file_writer,
                                          GArrowRecordBatch *record_batch,
                                          GError **error)
{
  auto arrow_file_writer =
    garrow_ipc_file_writer_get_raw(file_writer);
  auto arrow_record_batch =
    garrow_record_batch_get_raw(record_batch);
  auto arrow_record_batch_raw =
    arrow_record_batch.get();

  auto status = arrow_file_writer->WriteRecordBatch(*arrow_record_batch_raw);
  if (status.ok()) {
    return TRUE;
  } else {
    garrow_error_set(error, status, "[ipc][file-writer][write-record-batch]");
    return FALSE;
  }
}

/**
 * garrow_ipc_file_writer_close:
 * @file_writer: A #GArrowIPCFileWriter.
 * @error: (nullable): Return locatipcn for a #GError or %NULL.
 *
 * Returns: %TRUE on success, %FALSE if there was an error.
 */
gboolean
garrow_ipc_file_writer_close(GArrowIPCFileWriter *file_writer,
                             GError **error)
{
  auto arrow_file_writer =
    garrow_ipc_file_writer_get_raw(file_writer);

  auto status = arrow_file_writer->Close();
  if (status.ok()) {
    return TRUE;
  } else {
    garrow_error_set(error, status, "[ipc][file-writer][close]");
    return FALSE;
  }
}

G_END_DECLS

GArrowIPCFileWriter *
garrow_ipc_file_writer_new_raw(std::shared_ptr<arrow::ipc::FileWriter> *arrow_file_writer)
{
  auto file_writer =
    GARROW_IPC_FILE_WRITER(g_object_new(GARROW_IPC_TYPE_FILE_WRITER,
                                        "stream-writer", arrow_file_writer,
                                        NULL));
  return file_writer;
}

arrow::ipc::FileWriter *
garrow_ipc_file_writer_get_raw(GArrowIPCFileWriter *file_writer)
{
  auto arrow_stream_writer =
    garrow_ipc_stream_writer_get_raw(GARROW_IPC_STREAM_WRITER(file_writer));
  auto arrow_file_writer_raw =
    dynamic_cast<arrow::ipc::FileWriter *>(arrow_stream_writer.get());
  return arrow_file_writer_raw;
}

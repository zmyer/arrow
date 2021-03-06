# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

class TestStructArray < Test::Unit::TestCase
  def test_fields
    fields = [
      Arrow::Field.new("score", Arrow::Int8DataType.new),
      Arrow::Field.new("enabled", Arrow::BooleanDataType.new),
    ]
    data_type = Arrow::StructDataType.new(fields)
    field_builders = [
      Arrow::Int8ArrayBuilder.new,
      Arrow::BooleanArrayBuilder.new,
    ]
    builder = Arrow::StructArrayBuilder.new(data_type, field_builders)

    builder.append
    builder.get_field_builder(0).append(-29)
    builder.get_field_builder(1).append(true)

    builder.append
    builder.field_builders[0].append(2)
    builder.field_builders[1].append(false)

    array = builder.finish
    values = array.length.times.collect do |i|
      if i.zero?
        [
          array.get_field(0).get_value(i),
          array.get_field(1).get_value(i),
        ]
      else
        array.fields.collect do |field|
          field.get_value(i)
        end
      end
    end
    assert_equal([
                   [-29, true],
                   [2, false],
                 ],
                 values)
  end
end

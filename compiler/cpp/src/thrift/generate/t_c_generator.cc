/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 * Contains some contributions under the Thrift Software License.
 * Please see doc/old-thrift-license.txt in the Thrift distribution for
 * details.
 */

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <ctype.h>

#include "thrift/generate/t_oop_generator.h"
#include "thrift/platform.h"

#define D() std::cout << __func__ << "():" << __LINE__ << ": "

using std::map;
using std::ofstream;
using std::ostream;
using std::string;
using std::vector;

static const string endl = "\n"; // avoid ostream << std::endl flushes

class t_c_generator : public t_oop_generator {
public:
  t_c_generator(t_program* program,
                const map<string, string>& parsed_options,
                const string& option_string);
  ~t_c_generator();

  /**
   * Init and close methods
   */

  void init_generator() override;
  void close_generator() override;

  /**
   * Program-level generation functions
   */

  void generate_consts(vector<t_const*> consts) override;
  void generate_typedef(t_typedef* ttypedef) override;
  void generate_enum(t_enum* tenum) override;
  void generate_enum_ostream_operator_decl(std::ostream& out, t_enum* tenum);
  void generate_enum_ostream_operator(std::ostream& out, t_enum* tenum);
  void generate_enum_to_string_helper_function_decl(std::ostream& out, t_enum* tenum);
  void generate_enum_to_string_helper_function(std::ostream& out, t_enum* tenum);
  void generate_forward_declaration(t_struct* tstruct) override;
  void generate_struct(t_struct* tstruct) override;
  void generate_xception(t_struct* txception) override;
  void generate_cpp_struct(t_struct* tstruct, bool is_exception);

  void generate_service(t_service* tservice) override;

  void print_const_value(std::ostream& out, std::string name, t_type* type, t_const_value* value);
  std::string render_const_value(std::ostream& out,
                                 std::string name,
                                 t_type* type,
                                 t_const_value* value);

  void generate_struct_declaration(std::ostream& out,
                                   t_struct* tstruct,
                                   bool is_exception = false,
                                   bool pointers = false,
                                   bool read = true,
                                   bool write = true,
                                   bool swap = false,
                                   bool is_user_struct = false);
  void generate_struct_definition(std::ostream& out,
                                  std::ostream& force_cpp_out,
                                  t_struct* tstruct,
                                  bool setters = true,
                                  bool is_user_struct = false);
  void generate_copy_constructor(std::ostream& out, t_struct* tstruct, bool is_exception);
  void generate_move_constructor(std::ostream& out, t_struct* tstruct, bool is_exception);
  void generate_constructor_helper(std::ostream& out,
                                   t_struct* tstruct,
                                   bool is_excpetion,
                                   bool is_move);
  void generate_assignment_operator(std::ostream& out, t_struct* tstruct);
  void generate_move_assignment_operator(std::ostream& out, t_struct* tstruct);
  void generate_assignment_helper(std::ostream& out, t_struct* tstruct, bool is_move);
  void generate_struct_reader(std::ostream& out, t_struct* tstruct, bool pointers = false);
  void generate_struct_writer(std::ostream& out, t_struct* tstruct, bool pointers = false);
  void generate_struct_result_writer(std::ostream& out, t_struct* tstruct, bool pointers = false);
  void generate_struct_swap(std::ostream& out, t_struct* tstruct);
  void generate_struct_print_method(std::ostream& out, t_struct* tstruct);
  void generate_exception_what_method(std::ostream& out, t_struct* tstruct);

  /*
   * Service-level generation functions
   */
  void generate_service_interface(t_service* tservice);
  void generate_service_interface_factory(t_service* tservice);
  void generate_service_client(t_service* tservice);
  void generate_service_processor(t_service* tservice);
  void generate_service_skeleton(t_service* tservice);
  void generate_process_function(t_service* tservice, t_function* tfunction);
  void generate_function_helpers(t_service* tservice, t_function* tfunction);
  // void generate_service_async_skeleton(t_service* tservice);

  /*
   * Serialization constructs
   */

  void generate_deserialize_field(std::ostream& out, t_field* tfield);
  void generate_deserialize_struct(std::ostream& out, t_struct* tstruct);
  void generate_deserialize_container(std::ostream& out, t_type* ttype);
  void generate_deserialize_set_element(std::ostream& out, t_set* tset);
  void generate_deserialize_map_element(std::ostream& out, t_map* tmap);
  void generate_deserialize_list_element(std::ostream& out, t_list* tlist);
  void generate_function_call(ostream& out, t_function* tfunction, string target, string iface);
  void generate_serialize_field(std::ostream& out, t_field* tfield);
  void generate_serialize_struct(std::ostream& out, t_struct* tstruct);
  void generate_serialize_container(std::ostream& out, t_type* ttype);
  void generate_serialize_map_element(std::ostream& out, t_map* tmap, std::string iter);
  void generate_serialize_set_element(std::ostream& out, t_set* tset, std::string iter);
  void generate_serialize_list_element(std::ostream& out, t_list* tlist, std::string iter);

  /*
   * Helper rendering functions
   */

  std::string namespace_prefix(std::string ns);
  std::string namespace_open(std::string ns);
  std::string namespace_close(std::string ns);
  std::string type_name(t_type* ttype, bool in_typedef = false, bool arg = false);
  std::string base_type_name(t_base_type::t_base tbase);
  std::string declare_field(t_field* tfield,
                            bool init = false,
                            bool pointer = false,
                            bool constant = false,
                            bool reference = false);
  std::string function_signature(t_function* tfunction,
                                 std::string style,
                                 std::string prefix = "",
                                 bool name_params = true);
  std::string cob_function_signature(t_function* tfunction,
                                     std::string prefix = "",
                                     bool name_params = true);
  std::string argument_list(t_struct* tstruct, bool name_params = true, bool start_comma = false);
  std::string type_to_enum(t_type* ttype);

  void generate_enum_constant_list(std::ostream& f,
                                   const vector<t_enum_value*>& constants,
                                   const char* prefix,
                                   const char* suffix,
                                   bool include_values);

  void generate_struct_ostream_operator_decl(std::ostream& f, t_struct* tstruct);
  void generate_struct_ostream_operator(std::ostream& f, t_struct* tstruct);
  void generate_struct_print_method_decl(std::ostream& f, t_struct* tstruct);
  void generate_exception_what_method_decl(std::ostream& f,
                                           t_struct* tstruct,
                                           bool external = false);

  bool is_reference(t_field* tfield) { return tfield->get_reference(); }

  bool is_complex_type(t_type* ttype) {
    ttype = get_true_type(ttype);

    return ttype->is_container() || ttype->is_struct() || ttype->is_xception()
           || (ttype->is_base_type()
               && (((t_base_type*)ttype)->get_base() == t_base_type::TYPE_STRING));
  }

  // void set_use_include_prefix(bool use_include_prefix) { use_include_prefix_ =
  // use_include_prefix; }

  /**
   * The compiler option "no_thrift_ostream_impl" can be used to prevent
   * the compiler from emitting implementations for operator <<.  In this
   * case the consuming application must provide any needed to build.
   *
   * To disable this on a per structure bases, one can alternatively set
   * the annotation "cpp.customostream" to prevent thrift from emitting an
   * operator << (std::ostream&).
   *
   * See AnnotationTest for validation of this annotation feature.
   */
  //   bool has_custom_ostream(t_type* ttype) const {
  //     return (gen_no_ostream_operators_)
  //            || (ttype->annotations_.find("cpp.customostream") != ttype->annotations_.end());
  //   }

  /**
   * Determine if all fields of t_struct's storage do not throw
   * Move/Copy Constructors and Assignments applicable for 'noexcept'
   * Move defaults to 'noexcept'
   */
  bool is_struct_storage_not_throwing(t_struct* tstruct) const;

  bool use_malloc;

  ofstream f_types_;
  ofstream f_types_impl_;
  ofstream f_header_;
  ofstream f_service_;
};

t_c_generator::t_c_generator(t_program* program,
                             const map<string, string>& parsed_options,
                             const string& option_string)
  : t_oop_generator(program) {
  D() << "program: " << program->get_name() << endl;
  D() << "option_string: " << option_string << endl;
  (void)parsed_options;

  /*
    for (auto kv : parsed_options) {
      auto& key = kv.first;
      auto& val = kv.second;

      if (key == "no_malloc") {
        D() << "disabling malloc (val: '" << val << "')" << endl;
        use_malloc = false;
      } else {
        throw "unknown option c:" + key;
      }
    }
    */

  out_dir_base_ = "gen-c";
}

t_c_generator::~t_c_generator() {
  D() << endl;
}

void t_c_generator::init_generator() {
  D() << endl;

  MKDIR(get_out_dir().c_str());

  string f_types_name = get_out_dir() + program_name_ + "_types.h";
  D() << "f_types: " << f_types_name << endl;
  f_types_.open(f_types_name);

  string f_types_impl_name = get_out_dir() + program_name_ + "_types.c";
  D() << "f_types_impl: " << f_types_impl_name << endl;
  f_types_impl_.open(f_types_impl_name.c_str());

  f_types_ << autogen_comment();
  f_types_impl_ << autogen_comment();
}

void t_c_generator::close_generator() {
  D() << endl;

  f_types_.close();
  f_types_impl_.close();
}

void t_c_generator::generate_typedef(t_typedef* ttypedef) {
  D() << "ttypedef: " << ttypedef->get_name() << endl;
}

void t_c_generator::generate_enum(t_enum* tenum) {
  D() << "tenum: " << tenum->get_name() << endl;
}

void t_c_generator::generate_consts(vector<t_const*> consts) {
  D() << endl;
  string f_consts_name = get_out_dir() + program_name_ + "_constants.h";
  ofstream f_consts;

  f_consts.open(f_consts_name);

  string f_consts_impl_name = get_out_dir() + program_name_ + "_constants.c";
  ofstream_with_content_based_conditional_update f_consts_impl;
  f_consts_impl.open(f_consts_impl_name);

  // Print header
  f_consts << autogen_comment();
  f_consts_impl << autogen_comment();

  // Start ifndef
  f_consts << "#ifndef " << program_name_ << "_CONSTANTS_H" << endl
           << "#define " << program_name_ << "_CONSTANTS_H" << endl
           << endl
           << "#include \""
           // << get_include_prefix(*get_program())
           // << program_name_
           << "_types.h\""
           << endl
           //    << endl
           //           << ns_open_ << endl
           << endl;

  f_consts_impl << "#include \""
                // << get_include_prefix(*get_program())
                << program_name_ << "_constants.h\""
                << endl
                // << endl
                // << ns_open_ << endl
                << endl;

  for (auto& c : consts) {
    auto type = c->get_type();
    D() << "const " << type->get_name() << " " << c->get_name() << " = " << c->get_value() << endl;
  }

  f_consts
      << endl
      // needs definition
      //    << "extern const " << program_name_ << "Constants g_" << program_name_ << "_constants;"
      //    << endl

      // no namespaces
      //    << endl
      //    << ns_close_ << endl
      << endl
      << "#endif /* */" << endl;
  f_consts.close();

  f_consts_impl << endl
                // << ns_close_
                // << endl
                << endl;
  f_consts_impl.close();
}

void t_c_generator::generate_forward_declaration(t_struct* tstruct) {
  D() << "tstruct: " << tstruct->get_name() << endl;
}

void t_c_generator::generate_struct(t_struct* tstruct) {
  D() << "tstruct: " << tstruct->get_name() << endl;
}

void t_c_generator::generate_service(t_service* tservice) {
  D() << "tservice: " << tservice->get_name() << endl;

  string svcname = tservice->get_name();
  string f_header_name = get_out_dir() + svcname + ".h";
  f_header_.open(f_header_name.c_str());
  f_header_ << autogen_comment();

  f_header_ << "#ifndef " << svcname << "_H" << endl
            << "#define " << svcname << "_H" << endl
            << endl;

  f_header_ << "#endif /* " << svcname << "_H */" << endl;

  string f_service_name = get_out_dir() + svcname + ".c";
  f_service_.open(f_service_name.c_str());
  f_service_ << autogen_comment();
  f_service_ << "#include \"" << svcname << ".h\"" << endl;
  f_service_.close();
  f_header_.close();
}

void t_c_generator::generate_xception(t_struct* tstruct) {
  D() << "tstruct: " << tstruct->get_name() << endl;
}

THRIFT_REGISTER_GENERATOR(c, "C", "")

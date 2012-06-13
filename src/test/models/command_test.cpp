/** 
 * This test tries out different command line options for a 
 * generated model.
 *
 */
#include <gtest/gtest.h>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include <stan/mcmc/chains.hpp>
#include <bitset>

using std::stringstream;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;
using std::bitset;

using testing::Combine;
using testing::Range;

enum options {
  append_samples, // should be first option
  data,
  init,
  seed,
  chain_id,
  iter,
  warmup,
  thin,
  leapfrog_steps,
  max_treedepth,
  epsilon,
  epsilon_pm,
  // unit_max_matrix
  // delta
  // gamma
  options_count   // should be last. will hold the number of tested options
};


class ModelCommand : 
  public ::testing::TestWithParam<int> {
private:
  static char path_separator;
public:
  static string data_file_base;
  static string model_path;
  static vector<string> expected_help_options;
  static vector<pair<string, string> > expected_output;
  static vector<string> option_name;
  static vector<pair<string, string> > command_changes;
  static vector<pair<string, string> > output_changes;

  static char get_path_separator() {
    if (path_separator == 0) {
      FILE *in;
      if(!(in = popen("make path_separator --no-print-directory", "r")))
        throw std::runtime_error("\"make path_separator\" has failed.");
      path_separator = fgetc(in);
      pclose(in);
    }
    return path_separator;
  }
  
  static std::string get_path(const char* path_array[], size_t length) {
    std::string path;
    if (length==0)
      return path;
    
    path.append(path_array[0]);
    for (size_t i = 1; i < length; i++) {
      path.append(1, get_path_separator());
      path.append(path_array[i]);
    }
    return path;
  }



  static std::string get_path(const vector<string>& path_vector) {
    std::string path;
    if (path_vector.size()==0)
      return path;
    
    path.append(path_vector[0]);
    for (size_t i = 1; i < path_vector.size(); i++) {
      path.append(1, get_path_separator());
      path.append(path_vector[i]);
    }
    return path;
  }
  
  void static SetUpTestCase() {
    model_path.append("models");
    model_path.append(1, get_path_separator());
    model_path.append("command");

    data_file_base.append("src");
    data_file_base.append(1, get_path_separator());
    data_file_base.append("test");
    data_file_base.append(1, get_path_separator());
    data_file_base.append(model_path);

    expected_help_options.push_back("help");
    expected_help_options.push_back("data");
    expected_help_options.push_back("init");
    expected_help_options.push_back("samples");
    expected_help_options.push_back("append_samples");
    expected_help_options.push_back("seed");
    expected_help_options.push_back("chain_id");
    expected_help_options.push_back("iter");
    expected_help_options.push_back("warmup");
    expected_help_options.push_back("thin");
    expected_help_options.push_back("refresh");
    expected_help_options.push_back("leapfrog_steps");
    expected_help_options.push_back("max_treedepth");
    expected_help_options.push_back("epsilon");
    expected_help_options.push_back("epsilon_pm");
    expected_help_options.push_back("unit_mass_matrix");
    expected_help_options.push_back("delta");
    expected_help_options.push_back("gamma");
    expected_help_options.push_back("test_grad");

    expected_output.push_back(make_pair("data","(specified model requires no data)"));
    expected_output.push_back(make_pair("init", "random initialization"));
    expected_output.push_back(make_pair("init tries", "1"));
    expected_output.push_back(make_pair("samples", model_path+".csv"));
    expected_output.push_back(make_pair("append_samples", "0"));
    expected_output.push_back(make_pair("seed", ""));
    expected_output.push_back(make_pair("chain_id", "1 (default)"));
    expected_output.push_back(make_pair("iter", "2000"));
    expected_output.push_back(make_pair("warmup", "1000"));
    expected_output.push_back(make_pair("thin", "1 (default)"));
    expected_output.push_back(make_pair("unit_mass_matrix", "0"));
    expected_output.push_back(make_pair("leapfrog_steps", "-1"));
    expected_output.push_back(make_pair("max_treedepth", "10"));
    expected_output.push_back(make_pair("epsilon", "-1"));
    expected_output.push_back(make_pair("epsilon_pm", "0"));
    expected_output.push_back(make_pair("delta", "0.5"));
    expected_output.push_back(make_pair("gamma", "0.05"));
    
    option_name.resize(options_count);    
    command_changes.resize(options_count);
    output_changes.resize(options_count);

    option_name[append_samples] = "append_samples";
    command_changes[append_samples] = make_pair("",
                                                " --append_samples");
    output_changes [append_samples] = make_pair("",
                                                "1");

    option_name[data] = "data";
    command_changes[data] = make_pair(" --data="+data_file_base+"1.Rdata",
                                      " --data="+data_file_base+"2.Rdata");
    output_changes [data] = make_pair(data_file_base+"1.Rdata",
                                      data_file_base+"2.Rdata");

    option_name[init] = "init";
    command_changes[init] = make_pair("",
                                      " --init=" + data_file_base + "_init.Rdata");
    output_changes [init] = make_pair("",
                                      data_file_base + "_init.Rdata");


    option_name[seed] = "seed";
    command_changes[seed] = make_pair("",
                                      " --seed=100");
    output_changes [seed] = make_pair("",
                                      "100 (user specified)");


    option_name[chain_id] = "chain_id";
    command_changes[chain_id] = make_pair("",
                                          " --chain_id=2");
    output_changes [chain_id] = make_pair("",
                                          "2 (user specified)");
    
    option_name[iter] = "iter";
    command_changes[iter] = make_pair("",
                                      " --iter=100");
    output_changes [iter] = make_pair("",
                                      "100");

    option_name[warmup] = "warmup";
    command_changes[warmup] = make_pair("",
                                        " --warmup=60");
    output_changes [warmup] = make_pair("",
                                        "60");

    option_name[thin] = "thin";
    command_changes[thin] = make_pair("",
                                        " --thin=3");
    output_changes [thin] = make_pair("",
                                        "3 (user supplied)");



    option_name[leapfrog_steps] = "leapfrog_steps";
    command_changes[leapfrog_steps] = make_pair("",
						" --leapfrog_steps=1");
    output_changes [leapfrog_steps] = make_pair("",
						"1");
    

    option_name[max_treedepth] = "max_treedepth";
    command_changes[max_treedepth] = make_pair("",
					       " --max_treedepth=2");
    output_changes [max_treedepth] = make_pair("",
					       "2");
    
    option_name[epsilon] = "epsilon";
    command_changes[epsilon] = make_pair("",
					 " --epsilon=1.5");
    output_changes [epsilon] = make_pair("",
					 "1.5");
    

    option_name[epsilon_pm] = "epsilon_pm";
    command_changes[epsilon_pm] = make_pair("",
					    " --epsilon_pm=0.5");
    output_changes [epsilon_pm] = make_pair("",
					    "0.5");
    
    //for (int i = 0; i < options_count; i++) {
    //  std::cout << "\t" << i << ": " << option_name[i] << std::endl;
    //}
  }

  /** 
   * Runs the command provided and returns the system output
   * as a string.
   * 
   * @param command A command that can be run from the shell
   * @return the system output of the command
   */  
  string run_command(const string& command) {
    FILE *in;
    if(!(in = popen(command.c_str(), "r"))) {
      string err_msg;
      err_msg = "Could not run: \"";
      err_msg+= command;
      err_msg+= "\"";
      throw std::runtime_error(err_msg.c_str());
    }

    string output;
    char buf[1024];
    size_t count = fread(&buf, 1, 1024, in);
    while (count > 0) {
      output += string(&buf[0], &buf[count]);
      //std::cout << "intermediate output: " << output << std::endl;
      count = fread(&buf, 1, 1024, in);
    }
    pclose(in);
    
    //std::cout << "ran command: " << command << std::endl;
    //std::cout << "output : \n";
    //std::cout << output << std::endl << std::endl;
    return output;
  }

  /** 
   * Returns the help options from the string provide.
   * Help options start with "--".
   * 
   * @param help_output output from "model/command --help"
   * @return a vector of strings of the help options
   */
  vector<string> get_help_options(const string& help_output) {    
    vector<string> help_options;

    size_t option_start = help_output.find("--");
    while (option_start != string::npos) {
      // find the option name (skip two characters for "--")
      option_start += 2;
      size_t option_end = help_output.find_first_of("= ", option_start);
      help_options.push_back(help_output.substr(option_start, option_end-option_start));
      option_start = help_output.find("--", option_start+1);
    }
        
    return help_options;
  }
  
  vector<pair<string, string> > 
  parse_output(const string& command_output) {
    vector<pair<string, string> > output;

    string option, value;
    size_t start = 0, end = command_output.find("\n", start);
    
    EXPECT_EQ("STAN SAMPLING COMMAND", 
              command_output.substr(start, end))
      << "command could not be run. output is: \n" 
      << command_output;
    if ("STAN SAMPLING COMMAND" != command_output.substr(start, end)) {
      return output;
    }
    start = end+1;
    end = command_output.find("\n", start);
    size_t equal_pos = command_output.find("=", start);

    while (equal_pos != string::npos) {
      using boost::trim;
      option = command_output.substr(start, equal_pos-start);
      value = command_output.substr(equal_pos+1, end - equal_pos - 1);
      trim(option);
      trim(value);
      output.push_back(pair<string, string>(option, value));
      start = end+1;
      end = command_output.find("\n", start);
      equal_pos = command_output.find("=", start);
    }
    return output;
  }

  void check_output(const string& command_output,
                    const vector<pair<string, string> >& changed_options) {
    vector<pair<string, string> > expected_output(this->expected_output);
    for (size_t i = 0; i < changed_options.size(); i++) {
      for (size_t j = 0; j < expected_output.size(); j++) {
        if (expected_output[j].first == changed_options[i].first) {
          expected_output[j].second = changed_options[i].second;
          break;
        }
      }
    }
    for (size_t i = 0; i < changed_options.size(); i++) {
      if (changed_options[i].first == "init") {
        size_t j;
        for (j = 0; j < expected_output.size(); j++) {
          if (expected_output[j].first == "init tries") {
            break;
          }
        }
        expected_output.erase(expected_output.begin() + j);
      }
    }
    
    
    vector<pair<string, string> > output = parse_output(command_output);
    ASSERT_EQ(expected_output.size(), output.size());
    for (size_t i = 0; i < expected_output.size(); i++) {
      EXPECT_EQ(expected_output[i].first, output[i].first) <<
        "Order of output should match";
      if (expected_output[i].first == "seed" && expected_output[i].second == "") {
        // when seed is default, check to see that it is randomly generated
        if (boost::algorithm::ends_with(output[i].second, "(randomly generated)"))
          SUCCEED();
        else
          ADD_FAILURE() 
            << "'" << output[i].first 
            << "' is not randomly generated: " << output[i].second;
      } else {
        EXPECT_EQ(expected_output[i].second, output[i].second)
          << "Option '" << expected_output[i].first << "' returned unexpected value";
      }
        
    }

  }
  
  void check_output(const string& command_output) {
    vector<pair<string, string> > changed_options;
    check_output(command_output, changed_options);
  }
  
  string get_command(const bitset<options_count> options, 
                     vector<pair<string, string> > &changed_options) {
    stringstream command;
    command << model_path;
    command << " --samples=" + model_path + ".csv";

    //for (int i = options_count-1; i > -1; i--) {
    for (int i = 0; i < options_count; i++) {
      string output_option;
      if (!options[i]) {
        command << command_changes[i].first;
        output_option = output_changes[i].first;
      } else {
        command << command_changes[i].second;
        output_option = output_changes[i].second;
      }
      if (output_option != "") {
        changed_options.push_back(make_pair(option_name[i], 
                                            output_option));
      }
    }
    if (!options[warmup]) {
      int num_iter = options[iter] ? 100 : 2000;
      int num_warmup = options[warmup] ? 60 : num_iter/2;
      stringstream warmup;
      warmup << num_iter - num_warmup;
      changed_options.push_back(make_pair("warmup",
                                          warmup.str()));
    }
    
    return command.str();
  }
};

vector<string> ModelCommand::expected_help_options;
string ModelCommand::model_path;
string ModelCommand::data_file_base;
char ModelCommand::path_separator = 0;
vector<pair<string, string> > ModelCommand::expected_output;
vector<pair<string, string> > ModelCommand::command_changes;
vector<pair<string, string> > ModelCommand::output_changes;
vector<string> ModelCommand::option_name;


TEST_F(ModelCommand, HelpOptionsMatch) {
  string help_command = model_path;
  help_command.append(" --help");

  vector<string> help_options = 
    get_help_options(run_command(help_command));

  ASSERT_EQ(expected_help_options.size(), help_options.size());
  for (size_t i = 0; i < expected_help_options.size(); i++) {
    EXPECT_EQ(expected_help_options[i], help_options[i]);
  }
}

void test_sampled_mean(const bitset<options_count>& options, stan::mcmc::chains<> c) {
  double expected_mean = (options[data])*100.0; // 1: mean = 0, 2: mean = 100
  EXPECT_NEAR(expected_mean, c.mean(0U), 3)
    << "Test that data file is being used";
}

void test_number_of_samples(const bitset<options_count>& options, stan::mcmc::chains<> c) {
  int num_iter = options[iter] ? 100 : 2000;
  int num_warmup = options[warmup] ? 60 : num_iter/2;
  size_t expected_num_samples = num_iter - num_warmup;
  if (options[thin]) {
    expected_num_samples = ceil(expected_num_samples / 3.0);
  }
  if (options[append_samples]) {
    EXPECT_EQ(2*expected_num_samples, c.num_samples())
      << "Test number of samples when appending samples";
  } else {
    EXPECT_EQ(expected_num_samples, c.num_samples())
      << "Test number of samples when not appending samples";
  }
}

void test_specific_sample_values(const bitset<options_count>& options, stan::mcmc::chains<> c) {
  if (options[iter] || 
      options[leapfrog_steps] || 
      options[epsilon])
    return;
  // seed / chain_id test
  double expected_first_y;
  if (options[seed] && !options[append_samples] && !options[warmup]) {
    if (options[data]) {
      expected_first_y = options[init] ? 100.564 : 100.523;
    } else { 
      expected_first_y = options[init] ? 0.265544 : 1.76413;
    }
    
    vector<double> sampled_y;
    c.get_samples(0U, 0U, sampled_y);
    if (options[chain_id]) {
      if (expected_first_y == sampled_y[0]) {
        ADD_FAILURE()
          << "chain_id is not default. "
          << "sampled_y[0] should not be drawn from the same seed";
      } else {
        SUCCEED()
          << "chain_id is the default. "
          << "The samples are not drawn from the same seed";
      }
    } else {
      EXPECT_EQ(expected_first_y, sampled_y[0])
        << "Test for first sample when chain_id == 1";
    }
  }
}

TEST_P(ModelCommand, OptionsTest) {
  bitset<options_count> options(GetParam());
  vector<pair<string, string> > changed_options;
  
  std::string command = get_command(options, changed_options);
  SCOPED_TRACE(command);
  //std::cout << command << std::endl;
  //for (int i = 0; i < changed_options.size(); i++) {
  //  std::cout << i << ": " << changed_options[i].first << ", " << changed_options[i].second << std::endl;
  //}

  // check_output
  check_output(run_command(command), changed_options);

  // test sampled values
  vector<string> names;
  vector<vector<size_t> > dimss;
  size_t skip = options[leapfrog_steps] ? 1U : 2U;
  stan::mcmc::read_variables(model_path+".csv", skip,
                             names, dimss);
      
  stan::mcmc::chains<> c(1U, names, dimss);
  stan::mcmc::add_chain(c, 0, model_path+".csv", skip);
  
  test_sampled_mean(options, c);
  test_number_of_samples(options, c);
  test_specific_sample_values(options, c);
}
INSTANTIATE_TEST_CASE_P(,
                        ModelCommand,
                        Range(0, 1<<options_count));

//This is a part of tct, tct is a tool for counting text file.
//Copyright (C) 2016  Harry Leong(https://github.com/HarryLeong/tct)

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.


#ifndef __opts_opts
#define __opts_opts

#include <list>
#include <string>
#include <type_traits>
#include <vector>
#include <sstream>

namespace opts {

	struct Opts
	{
	private:
		struct Opt
		{
			Opt(std::string const &lf,
				std::string const &sf,
				std::string const &bref,
				std::string const &detail)
			{
				longf = lf;
				shortf = sf;
				this->bref = bref;
				this->detail = detail;
			}
			virtual bool handle(std::string const &str,std::string *err) = 0;

			void help(std::string &buf)
			{
				if(!shortf.empty()) {
					buf.append("-").append(shortf).append(" ");
				}
				if(!longf.empty()) {
					buf.append("--").append(longf).append(" ");
				}
				buf.append(":");
				onType(buf);
				buf.append("\n");
				if(!bref.empty()) {
					buf.append(bref);
					if(bref.back() != '\n') {
						buf.append("\n");
					}
				}
				if(!detail.empty()) {
					buf.append("\n");
					buf.append(detail);
					if(detail.back() != '\n') {
						buf.append("\n");
					}
				}
				//double '\n'
				buf.append("\n");
			}
			virtual void onType(std::string &str) = 0;
			
			virtual ~Opt()
			{ }

			std::string longf;
			std::string shortf;
			std::string bref;
			std::string detail;
		private:
		};

		template< class T >
		struct TypeString
		{
			char const * getString()
			{ return "BAD TYPE"; }
		};

		template<>
		struct TypeString<bool>
		{
			char const * getString()
			{ return "bool"; }
		};

		template<>
		struct TypeString<int>
		{
			char const * getString()
			{ return "int"; }
		};

		template< >
		struct TypeString<std::string>
		{
			char const * getString()
			{ return "stirng"; }
		};

		template< class T, class BoolTakeValueAndString >
		struct OptT : Opt
		{
			template<class BoolTakeValueAndStringA>
			OptT(std::string const &lf,
				std::string const &sf,
				std::string const &bref,
				BoolTakeValueAndStringA &&fun,
				std::string const &detail) : Opt(lf, sf, bref, detail)
				,mFun(std::forward<BoolTakeValueAndStringA>(fun))
			{
			}

			bool handle(std::string const &arg,std::string *err) override
			{
				T value;
				if(arg.empty()) {
					if(std::is_same<T,bool>::value) {
						value = true;
					} else {
						err->append("Non-bool type option needs sub-option");
						return false;
					}
				} else {
					std::stringstream stream(arg);
					stream >> value;
					if(!stream.eof()) {
						err->append("Failed to parse sub-option");
						return false;
					}
				}
				T const &cv = value;
				if(!mFun(cv, err)) {
					return false;
				}
				return true;
			}

			void onType(std::string &str) override
			{
				str.append(TypeString<T>().getString());
			}

		private:
			BoolTakeValueAndString mFun;
		};
		
		template<class T>
		struct Tt {
			typedef T type;
		};
		template<>
		struct Tt<char const *> {
			typedef std::string type;
		};

		template<>
		struct Tt<char *> {
			typedef std::string type;
		};

		template<int N>
		struct Tt<char const[N]> {
			typedef std::string type;
		};

		template<int N>
		struct Tt<char [N]> {
			typedef std::string type;
		};
	public:
		struct Error
		{
			std::string const &string()
			{ return mStr; }

			int code()
			{ return mCode; }

			Error(Error &&err) : mStr(std::move(err.mStr))
			{ mCode = err.mCode; }

		private:
			friend struct Opts;
			Error()
			{ mCode = 0; }
			int mCode;
			std::string mStr;
		};

		Opts()
		{
			mCallHelp = false;
			mToolname = "toolname";
		}

		~Opts()
		{
			for(Opt *&opt : mOpts) {
				delete opt;
				opt = nullptr;
			}
		}

		template< class T ,class BoolTakeValueAndString >
		Opts &add(std::string const &lf,
			std::string const &sf,
			std::string const &bref,
			T const &type_hint,
			BoolTakeValueAndString &&fun,
			std::string const &detail = std::string())
		{
			if(mCallHelp) {
				throw std::exception("Opts::add(...) failed; Because you called help() before calling add(..).\n");
			}
			typedef OptT<
				typename Tt<typename std::remove_reference<T>::type>::type,
				typename std::remove_reference<BoolTakeValueAndString>::type
			> Optt;
			Opt *ptr = new Optt(lf,sf,bref,std::forward<BoolTakeValueAndString>(fun),detail);
			mOpts.push_back(ptr);
			return *this;
		}

		std::string help()
		{
			mCallHelp = true;
			std::string buf;
			buf.append("\n");
			if(!mDescription.empty()) {
				buf.append(mDescription);
				if(mDescription.back()!='\n') {
					buf.append("\n");
				}
			}
			buf.append("usage:\n")
				.append(mToolname)
				.append(" [option=sub-option | option:sub-option] arguemnts\n"
				"you must specify sub-options except option is bool type\n\n");
			for(auto opt : mOpts) {
				opt->help(buf);
			}
			return buf;
		}

		template<class String>
		void description(String &&string)
		{
			if(mCallHelp) {
				throw std::exception("Opts::description(...) failed; Because you called help() before calling description(..).\n");
			}
			mDescription = std::forward<String>(string);
		}

		template<class String>
		void toolname(String &&string)
		{
			if(mCallHelp) {
				throw std::exception("Opts::toolname(...) failed; Because you called help() before calling toolname(..).\n");
			}
			mToolname = std::forward<String>(string);
		}


		Error parse(int argc, char **argv, std::vector<char*> *reset)
		{
			Error err;
			int end = argc;
			reset->push_back( argv[0]);
			for(int index = 1;index < end; ++index) {
				char * const arg = argv[index];
				if(arg[0] == '-') {
					std::string sarg;
					bool iss;
					if(arg[1] =='-') {
						sarg.assign(arg + 2);
						iss = false;
					} else {
						sarg.assign(arg + 1);
						iss = true;
					}

					if(sarg.empty()) {
						++err.mCode;
						err.mStr.append("Empty option!\n");
						break;
					}
					bool handled = false;
					bool herr = false;
					for(Opt *opt: mOpts) {
						std::string const *pflag = nullptr; 
						if(iss) {
							pflag = &opt->shortf;
						} else {
							pflag = &opt->longf;
						}
						std::string const &flag = *pflag;
						if(flag.empty()) {
							continue;
						}
						if(startWith(sarg, flag)) {
							sarg.erase(0, flag.size());
							if(sarg.size() == 0 || sarg[0] ==':' || sarg[0] =='=') {
								if(sarg.size() > 0) {
									sarg.erase(0, 1);
								}
								handled = true;
								std::string inf;
								if(!opt->handle(sarg, &inf)) {
									++err.mCode;
									err.mStr.append("Error option(")
										.append(arg)
										.append("): ")
										.append(inf)
										.append("\n");
								}

								break;
							}
						}
					}
					if(!handled) {
						++err.mCode;
						err.mStr.append("Unhandled option(")
							.append(arg)
							.append(")")
							.append("\n");
					}
				} else {
					reset->push_back(arg);
				}
			}
			return std::move(err);
		}
	private:

		bool startWith(std::string const &str, std::string const &substr)
		{
			if(substr.size() > str.size()) {
				return false;
			}
			for(int i = 0; i < (int)substr.size(); ++i) {
				if(substr[i] != str[i]) {
					return false;
				}
			}
			return true;
		}

		std::string mToolname;
		std::string mDescription;
		bool mCallHelp;
		std::list<Opt*> mOpts;
	};

}

#endif

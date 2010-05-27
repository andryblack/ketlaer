/* SQLitepp multilanguage object oriented wrapper to sqlite3 database library
 * Copyright (C) 2004 Alessandro Molina <amol@sig11.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#include "cpp_sqlitepp.hpp"

#include "libbitap.h"
#include "common.hpp"

#include <string>
#include <vector>
#include <iostream>

SQLRow::SQLRow(SQLQuery *lq)
: linkedq_(lq), changed_(false) {}

void SQLRow::addValue(char const *val)
{
	was_.push_back(val);
	fieldmod_.push_back(false);
	data_.push_back(val);
}

unsigned int SQLRow::numberOfColumns()
{
    return data_.size();
}

int SQLQuery::idOf(char const *fieldName)
{
	for(unsigned int i=0; i<colnames_.size(); ++i) {
		if(colnames_[i] == fieldName)
			return i;
	}
	return -1;
}

SQLRow *SQLQuery::getRow(unsigned int i)
{
	if(i<rows_.size())
		return rows_[i];
	else
		return 0;
}

sqlite3 *SQLQuery::referencedDb()
{
	return db_;
}

char const *SQLRow::get(char const *fieldName)
{
    int index = linkedq_->idOf(fieldName);
    if(index == -1)
        return 0;
    else
        return data_[index].c_str();
}

char const *SQLRow::getIdx(unsigned int n)
{
    if(n < data_.size())
        return data_[n].c_str();
    else
        return 0;
}

std::string &SQLRow::operator[](std::string const &fname)
{
	static std::string empty_string = "";

	int index = linkedq_->idOf(fname.c_str());
	if (index == -1)
		return empty_string;
	else
		return data_[index];
}

	
void SQLRow::set(char const *fieldName, char const *value)
{
	int i = linkedq_->idOf(fieldName);
	if(i==-1)
		return;

	changed_ = true;
	if(fieldmod_[i] == false) {
		was_[i] = data_[i];
		fieldmod_[i] = true;
	}
	data_[i] = value;
}

void SQLRow::setIdx(unsigned int i, char const *value)
{
    if(i >= data_.size())
        return;
        
    changed_ = true;
    if(fieldmod_[i] == false) {
    was_[i] = data_[i];
		fieldmod_[i] = true;
	}
	data_[i] = value;
}

bool SQLRow::commit(const std::string& whereclause)
{
	if(!changed_)
		return true;
	
	std::string sep = " ";
	std::string sql = std::string("UPDATE ") + linkedq_->tableName() + " SET";
	for(int i=0; i<linkedq_->numberOfColumns(); ++i) {
		if(i==0)
			sep = " ";
		else
			sep = ", ";
		sql += sep + linkedq_->nameOfColumn(i) + " = ";
		if(data_[i].size())
		  sql += std::string("'") + string_format::escape_db_string(data_[i]) + "'";
		else
			sql += "''";
	}
	
	sql += " WHERE";
	if (whereclause == "")
	for(int i=0; i<linkedq_->numberOfColumns(); ++i) {
		if(i==0)
			sep = " ";
		else
			sep = " and ";
		sql += sep + linkedq_->nameOfColumn(i) + " = ";
		if(was_[i].size()) {
		  sql += std::string("'") + string_format::escape_db_string(was_[i]) + "'";
		}
		else {
			sql += "''";
		}
	}
	else
	  sql+= " " + whereclause;

	
#ifdef DEBUG
	std::cerr << "SET GENERATED SQL: " << sql << std::endl;
#endif
	char *sqlerr;	
	int rc = sqlite3_exec(linkedq_->referencedDb(), sql.c_str(), 0, 0, &sqlerr);
	if(rc == SQLITE_OK) {
		changed_ = false;
		for(unsigned int i=0; i<fieldmod_.size(); ++i)
			fieldmod_[i] = false;
	}
	else{
	  std::cerr << "SQLITE ERROR: " << rc << " - " << sqlerr << std::endl;
	  sqlite3_free(sqlerr);
	}
	return ( rc==SQLITE_OK );
}

char const *SQLQuery::tableName()
{
	return tablename_.c_str();
}

int SQLQuery::numberOfColumns()
{
	return ncols_;
}

int SQLQuery::numberOfTuples()
{
    return rows_.size();
}

char const *SQLQuery::nameOfColumn(unsigned int n)
{
	return colnames_[n].c_str();
}

SQLQuery::SQLQuery(sqlite3 *db, char const *tablename)
: db_(db), ncols_(0), tablename_(tablename)
{}
	
SQLQuery::~SQLQuery()
{
	for(RowVect::iterator i=rows_.begin(); i!=rows_.end(); ++i) {
		delete *i;
	}
}

void SQLQuery::addRow(SQLRow *row)
{
	rows_.push_back(row);
}

void SQLQuery::setColNames(int ncols, char const * const *colnames)
{
	if(ncols_==0) {
		for(;ncols_<ncols; ++ncols_)
			colnames_.push_back(colnames[ncols_]);
	}
}

int SQLDatabase::last_index()
{
  return sqlite3_last_insert_rowid(db_);
}

int SQLDatabase::Callback(void *self, int argc, char **argv, char **columnNames)
{
	SQLRow *row = new SQLRow(((SQLDatabase*)self)->query_ris_);

	for(int i=0; i<argc; ++i)
		row->addValue(argv[i] ? argv[i] : "NULL");

	((SQLDatabase*)self)->query_ris_->addRow(row);
	((SQLDatabase*)self)->query_ris_->setColNames(argc, columnNames);
	return 0;
}

SQLDatabase::SQLDatabase(char const *filename)
: query_ris_(0)
{
  static bool limit_set = false;
  if (!limit_set) {
    sqlite3_soft_heap_limit(128*1024);
    limit_set = true;
  }
  sqlite3_open(filename, &db_);	
  sqlite3_create_function(db_, "fuzzycmp", 3, SQLITE_UTF8, 0, fuzzy_cmp_func, 0, 0);
  //  sqlite_function_type(db_, "fuzzycmp", SQLITE_TEXT);
}

void replace_table(char const *table, std::string &cmd)
{
	std::string::size_type tp = 0;

	while( (tp = cmd.find("%", tp)) != std::string::npos) {
		if((tp+1) >= cmd.size())
			return;

		if(cmd[tp+1] == '%')
			cmd.replace(tp++, 2, "%");
		else if(cmd[tp+1] == 't') {
			cmd.replace(tp, 2, table);
			tp += strlen(table);
		} else {
		  std::cerr << "invalid string, please escape % using %%" << std::endl;
		  return;
		}
	}
}

void SQLDatabase::fuzzy_cmp_func(sqlite3_context *context, int argc, sqlite3_value** argv)
{
  if(argc!=3) 
    return;

  const unsigned char *Pattern = sqlite3_value_text(argv[0]);
  const unsigned char *Text = sqlite3_value_text(argv[1]);

  const char *ccPattern = (const char *) Pattern;

  if(!Pattern || !Text) {
    sqlite3_result_int(context, 0);
    return;
  }

  int Result = 0;

  if (strlen(ccPattern) > 0) {

    bitapType b;
    int AllowedErrors = sqlite3_value_int(argv[2]);
    const char *ccText = (const char *) Text;

    if (strstr(ccText, ccPattern) != 0)
      Result = 1;
    else if (NewBitap(&b, Pattern) >= 0) {
      if (FindWithBitap(&b, Text, strlen(ccText), AllowedErrors,NULL,NULL) != NULL)
	Result = 1;
      
      DeleteBitap(&b);
    }
  } else
    Result = 1;

  sqlite3_result_int(context,Result);
}

SQLQuery *SQLDatabase::query(char const *tablename, char const *command)
{
        char *sqlerr;
	query_ris_ = new SQLQuery(db_, tablename);
	
	std::string cmd = command;
	replace_table(tablename, cmd);

#ifdef DEBUG
	std::cerr << "QUERY GENERATED SQL: " << cmd << std::endl;
#endif
	
	int ris = sqlite3_exec(db_, cmd.c_str(), Callback, this, &sqlerr);

	if( ris != SQLITE_OK ) {
	        std::cerr << "SQLITE ERROR: " << ris << " - " << sqlerr << std::endl;
                sqlite3_free(sqlerr);
		delete query_ris_;
		query_ris_ = NULL;
		return NULL;
	}
	
	return query_ris_;
}

bool SQLDatabase::hasTable(char const *tablename)
{
	std::string ask_query = "SELECT type FROM %t WHERE (name = \"";
	ask_query+=tablename;
	ask_query+="\") AND (type = \"table\")";

	SQLQuery * q = query("sqlite_master", ask_query.c_str());
 	if (q && q->numberOfTuples()) { delete q; return true; }
	return false;
}

bool SQLDatabase::insert(char const *tablename, std::vector<std::string> const &data)
{
	std::string sql = "INSERT INTO ";
	sql+=tablename;
	sql+=" VALUES(\"";

	for(unsigned int i=0; i < data.size()-1; ++i)
		sql+=data[i]+"\", \"";

	sql+=data[data.size()-1]+"\")";

#ifdef DEBUG
	std::cerr << "INSERT GENERATED SQL: " << sql << std::endl;
#endif
	return execute(sql.c_str());
}

bool SQLDatabase::mapInsertion(char const * source_table, char const * dest_table)
{
	SQLQuery * source_q = query(source_table, "PRAGMA table_info(%t)");
	SQLQuery * dest_q = query(dest_table, "PRAGMA table_info(%t)");
	if ( (!source_q) || (!dest_q) ) {
		if (source_q) delete source_q;
		if (dest_q) delete dest_q;
		return false;
	}
	
	std::vector<std::string> valid_columns;
	int dest_size = dest_q->numberOfTuples();
	for (int i=0; i<dest_size; ++i)
		valid_columns.push_back( dest_q->getRow(i)->getIdx(1) );

	SQLQuery * source_data = query(source_table, "SELECT * FROM %t");
	for (int row_id = 0; row_id < source_data->numberOfTuples(); ++row_id) {
		SQLRow * row = source_data->getRow(row_id);
		std::vector<std::string> insert_values;
		
		for (std::vector<std::string>::iterator cur_col_name = valid_columns.begin(); cur_col_name != valid_columns.end(); ++cur_col_name) {
			char const * val = row->get(cur_col_name->c_str());
			if (val)
				insert_values.push_back(val);
			else
				insert_values.push_back("");
		}

		insert(dest_table, insert_values);
	}

	delete source_q;
	delete dest_q;
	delete source_data;

	return true;
}

bool SQLDatabase::createTable(char const * tablename, std::vector<std::string> const &data)
{
	if (hasTable(tablename)) return false;
	if ( (data.empty()) || (!tablename) || (*tablename == '\0') ) return false;
	
	std::string sql = "CREATE TABLE '";
	sql+=tablename;
	sql+="' (";
	
	std::vector<std::string>::const_iterator last = data.end()-1;
	for(std::vector<std::string>::const_iterator i = data.begin(); i != last; ++i) {
		sql += *i;
		sql += ", ";
	}
	sql += *last;
	sql += ")";

#ifdef DEBUG
	std::cerr << "INSERT GENERATED SQL: " << sql << std::endl;
#endif
	return execute(sql.c_str());
}

bool SQLDatabase::deleteTable(char const * tablename)
{
	std::string sql = "DROP TABLE '";
	sql+=tablename;
	sql+="'";
	return execute(sql.c_str());
}

bool SQLDatabase::renameTable(char const * tablename, char const *newname)
{
	std::string sql = "ALTER TABLE '";
	sql+=tablename;
	sql+="' RENAME TO '";
	sql+=newname;
	sql+="'";
	return execute(sql.c_str());
}

bool SQLDatabase::addColumnToTable(char const * tablename, char const *columndef)
{
	std::string sql = "ALTER TABLE '";
	sql+=tablename;
	sql+="' ADD COLUMN ";
	sql+=columndef;
	return execute(sql.c_str());
}

bool SQLDatabase::execute(char const *sqlcmd)
{
	if( sqlite3_exec(db_, sqlcmd, 0, 0, 0) != SQLITE_OK)
		return false;
	return true;
}

SQLQuery *SQLDatabase::lastQuery() const
{
	return query_ris_;
}
  
void SQLDatabase::close()
{
  if (db_ != 0) {
    sqlite3_close(db_);
    db_ = 0;
  }
}	

SQLDatabase::~SQLDatabase()
{
  close();
}	

bool SQLDatabase::operator!() const
{
	return (db_==0);
}

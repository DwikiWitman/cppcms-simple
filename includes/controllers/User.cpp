#include "User.h"
#include <cppcms/service.h>  
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/http_response.h>
#include <cppdb/frontend.h>
#include <picojson.h>

// constructor
User::User(cppcms::service &srv) : Master(srv)
{
    dispatcher().assign("", &User::getUser, this);
    mapper().assign("");

    dispatcher().assign("/(\\d+)", &User::getUserById, this, 1);
    mapper().assign("/{1}");
}

void User::getUser()
{
    response().out() <<"get all users";
}

void User::getUserById(std::string id)
{
    // response().out() <<"get user by id "<< id;
    // Query the database to fetch the user by ID
    cppdb::result res = sql() << "SELECT * FROM users WHERE usrsId = ?" << id;
    
    if (res.next()) {
        // Fetch the details of the user
        int colFirstName = res.find_column("usrsFirstName");
        std::string firstName;
        res.fetch(colFirstName, firstName);
        
        int colLastName = res.find_column("usrsLastName");
        std::string lastName;
        res.fetch(colLastName, lastName);

        int colLoginId = res.find_column("usrsLoginId");
        std::string loginId;
        res.fetch(colLoginId, loginId);

        int colCreatedTime = res.find_column("usrsCreatedTime");
        std::string createdTime;
        res.fetch(colCreatedTime, createdTime);

        int colUpdatedTime = res.find_column("usrsUpdatedTime");
        std::string updatedTime;
        res.fetch(colUpdatedTime, updatedTime);

        int colIsBlocked = res.find_column("usrsIsBlocked");
        int isBlockedInt;
        res.fetch(colIsBlocked, isBlockedInt);
        bool isBlocked = (isBlockedInt != 0);

        int colDeletedTime = res.find_column("usrsDeletedTime");
        std::string deletedTime;
        res.fetch(colDeletedTime, deletedTime);

        // Create a JSON object for the user details
        picojson::object userJson;
        userJson["id"] = picojson::value(id);
        userJson["firstName"] = picojson::value(firstName);
        userJson["lastName"] = picojson::value(lastName);
        userJson["loginId"] = picojson::value(loginId);
        userJson["createdTime"] = picojson::value(createdTime);
        userJson["updatedTime"] = picojson::value(updatedTime);
        userJson["isBlocked"] = picojson::value(isBlocked);
        userJson["deletedTime"] = picojson::value(deletedTime);

        picojson::value jsonResponse(userJson);
        
        // Send the response as JSON
        response().set_header("Content-Type", "application/json");
        response().out() << jsonResponse.serialize();
    } else {
        // If no user is found with the provided ID
        response().out() << "User not found with ID: " << id;
    }
}

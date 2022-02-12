#include <iostream>
#include "logmine.h"
// #include "rapidjson/document.h"
#include "sajson.h"

auto success(const sajson::document& doc) -> bool {
    if (!doc.is_valid()) {
        fprintf(stderr, "%s\n", doc.get_error_message_as_cstring());
        return false;
    }
    return true;
}

auto main(int argc, char* argv[]) -> int {

    auto logs = std::string{R"([
        { "message": "2020-09-06T16:00:00 Disconnected from broker broker1" },
        { "message": "2020-09-06T16:00:00 Disconnected from broker broker2" },
        { "message": "2020-09-06T16:00:00 Disconnected from broker broker3" },
        { "message": "2020-09-06T16:00:00 Disconnected from broker broker4" }
    ])"};

    // rapidjson::Document d;
    // d.Parse(logs);

    const sajson::document& document = sajson::parse(
        sajson::dynamic_allocation(), sajson::mutable_string_view(sajson::string(logs.c_str(), logs.length())));
    if (!success(document)) {
        // fclose(file);
        // delete[] buffer;
        return 1;
    }

    Logmine model;

    const sajson::value& root = document.get_root();
    for (auto i = 0; i < root.get_length(); ++i) {
        const sajson::value& e = root.get_array_element(i);
        const sajson::value& message = e.get_value_of_key(sajson::literal("message"));
        model.add(message.as_cstring());
    }

    // for (auto& log : d.GetArray()) {
    //     model.add(log["message"].GetString());
    // }

    return 0;
}
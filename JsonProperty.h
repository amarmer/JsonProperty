#pragma once

#include <string>
#include <vector>
#include <map>

/// Json-cpp amalgated header (http://jsoncpp.sourceforge.net/).
#define JSON_IS_AMALGAMATION 0
#include "json.h"

#define JSON_PROPERTY(t, m, k)  JsonProperty<t> m{key(k)}; 

namespace Json
{
    // 'toJsonValue' functions convert a 'value' to 'Json::Value'
    template <typename T>
    inline Json::Value toJsonValue(const T& value)
    {
        return value;
    }

    template <typename T>
    inline Json::Value toJsonValue(const std::vector<T>& value)
    {
        Json::Value json;

        for (const auto& el : value)
        {
            json.append(el);
        }

        return json;
    }

    template <typename T>
    inline Json::Value toJsonValue(const std::map<std::string, T>& value)
    {
        Json::Value json;

        for (auto it = value.begin(); it != value.end(); ++it)
        {
            json[it->first] = it->second;
        }

        return json;
    }


    // 'Init::init' is used to initialize 'Json::Data' and 'JsonProperty' 
    template <bool isData, typename T>
    struct Init
    {
        static void init(T& t, const Json::Value& jsonValue)
        {
            (Data&)t = jsonValue;
        }
    };

    template <typename T>
    struct Init<false, T>
    {
        static void init(T& t, const Json::Value& jsonValue)
        {
            t = Utils<T>::fromJsonValue(jsonValue);
        }
    };


    // 'Utils' has 'fromJsonValue' and 'isValid' functions which are implememented for Json all types
    template <typename T>
    struct Utils {};

    template <>
    struct Utils<std::string>
    {
        static std::string fromJsonValue(const Json::Value& jsonValue)
        {
            return jsonValue.asString();
        }

        static bool isValid(const Json::Value& jsonValue)
        {
            return jsonValue.isString();
        }
    };

    template <>
    struct Utils<int>
    {
        static int fromJsonValue(const Json::Value& jsonValue)
        {
            return jsonValue.asInt();
        }

        static bool isValid(const Json::Value& jsonValue)
        {
            return jsonValue.isInt();
        }
    };

    template <>
    struct Utils<unsigned int>
    {
        static int fromJsonValue(const Json::Value& jsonValue)
        {
            return jsonValue.asUInt();
        }

        static bool isValid(const Json::Value& jsonValue)
        {
            return jsonValue.isUInt();
        }
    };

    template <>
    struct Utils<bool>
    {
        static bool fromJsonValue(const Json::Value& jsonValue)
        {
            return jsonValue.asBool();
        }

        static bool isValid(const Json::Value& jsonValue)
        {
            return jsonValue.isBool();
        }
    };

    template <>
    struct Utils<float>
    {
        static float fromJsonValue(const Json::Value& jsonValue)
        {
            return jsonValue.asFloat();
        }

        static bool isValid(const Json::Value& jsonValue)
        {
            return jsonValue.isDouble();
        }
    };

    template <>
    struct Utils<double>
    {
        static double fromJsonValue(const Json::Value& jsonValue)
        {
            return jsonValue.asDouble();
        }

        static bool isValid(const Json::Value& jsonValue)
        {
            return jsonValue.isDouble();
        }
    };

    template <>
    struct Utils<Json::Value>
    {
        static Json::Value fromJsonValue(const Json::Value& jsonValue)
        {
            return jsonValue;
        }
    };

    template <typename T>
    struct Utils<std::vector<T>>
    {
        static std::vector<T> fromJsonValue(const Json::Value& jsonValue)
        {
            std::vector<T> ret;

            for (auto it = jsonValue.begin(); it != jsonValue.end(); ++it)
            {
                T t;
                Init<std::is_base_of<Data, T>::value, T>::init(t, jsonValue[it.index()]);

                ret.push_back(t);
            }

            return ret;
        }

        static bool isValid(const Json::Value& jsonValue)
        {
            return jsonValue.isArray();
        }
    };

    template <typename T>
    struct Utils<std::map<std::string, T>>
    {
        static std::map<std::string, T> fromJsonValue(const Json::Value& jsonValue)
        {
            std::map<std::string, T> ret;

            for (auto it = jsonValue.begin(); it != jsonValue.end(); ++it)
            {
                string key = it.key().asString();

                T t;
                Init<std::is_base_of<Data, T>::value, T>::init(t, jsonValue[key]);

                ret.insert(make_pair(key, t));
            }

            return ret;
        }

        static bool isValid(const Json::Value& jsonValue)
        {
            return jsonValue.isObject();
        }
    };


    // 'Info' is common auxiliary for 'Json::Data' and 'JsonProperty'
    struct Info
    {
        Info()
            : pInfo_(nullptr)
        {}

        Info(const Info& info)
            : pInfo_(info.pInfo_), key_(info.key_)
        {}

        Info(Info* pInfo, const std::string& key)
            : pInfo_(pInfo), key_(key)
        {}

        Info(const Json::Value& jsonValue)
            :pInfo_(nullptr), jsonValue_(jsonValue)
        {}

        const Json::Value& jsonValue() const
        {
            return JsonValue<const Json::Value&>(this);
        }

        Json::Value& jsonValue()
        {
            return JsonValue<Json::Value&>(this);
        }

    private:
        template <typename Ret, typename T>
        static Ret JsonValue(T* pInfo)
        {
            if (!pInfo->pInfo_)
                return pInfo->jsonValue_;

            if (pInfo->pInfo_->pInfo_)
                return pInfo->pInfo_->jsonValue()[pInfo->key_];

            return pInfo->pInfo_->jsonValue_[pInfo->key_];
        }

    protected:
        Info* pInfo_;
        std::string key_;
        Json::Value jsonValue_;
    };


    // 'JsonPropertyBase' is base class of 'JsonProperty' if 'T' is derived from 'Json::Data'
    template <bool isData, typename T>
    struct JsonPropertyBase: public T
    {
        // Copy constructor of 'JsonPropertyBase' to avoid shallow copy of 'pInfo_' 
        JsonPropertyBase(const JsonPropertyBase& jsonPropertyBase)
        {
            pInfo_ = (Info*)((char*)this + ((char*)jsonPropertyBase.pInfo_ - (char*)&jsonPropertyBase));

            key_ = jsonPropertyBase.key_;
        }

        JsonPropertyBase(const Info& info)
        {
            (Info&)(*this) = info;
        }

        void operator = (const JsonPropertyBase& jsonPropertyBase)
        {
            jsonValue() = jsonPropertyBase.jsonValue();
        };

        void operator = (const T& t)
        {
            jsonValue() = t.jsonValue();
        };

        void operator = (const Json::Value& json)
        {
            jsonValue() = json;
        };
    };


    // 'JsonPropertyBase' is base class of 'JsonProperty' if 'T' is basic Json type
    template <typename T>
    struct JsonPropertyBase<false, T>: public Info
    {
        // Copy constructor of 'JsonPropertyBase' to avoid shallow copy of 'pInfo_' 
        JsonPropertyBase(const JsonPropertyBase& jsonPropertyBase)
        {
            pInfo_ = (Info*)((char*)this + ((char*)jsonPropertyBase.pInfo_ - (char*)&jsonPropertyBase));

            key_ = jsonPropertyBase.key_;
        }

        JsonPropertyBase(const Info& info)
            : Info(info)
        {}

        operator T() const
        {
            return Utils<T>::fromJsonValue(jsonValue());
        };

        void operator = (const T& t)
        {
            jsonValue() = toJsonValue(t);
        };

        bool isValid() const
        {
            return Utils<T>::isValid(jsonValue());
        }
    };


    // A class which maps Json to C++ structure should be derived from 'Json::Data' 
    struct Data : public Info
    {
    protected:
        // A member of class derived from 'Json::Data' and accesses Json should type 'JsonProperty'
        template <typename T>
        struct JsonProperty : public JsonPropertyBase<std::is_base_of<Data, T>::value, T>
        {
            JsonProperty(const Info& info)
                : JsonPropertyBase<std::is_base_of<Data, T>::value, T>(info)
            {}

            void remove()
            {
                pInfo_->jsonValue().removeMember(key_.c_str());
            }

            bool isNull() const
            {
                jsonValue().isNull();
            }

            bool isEmpty() const
            {
                jsonValue().isEmpty();
            }

            using JsonPropertyBase<std::is_base_of<Data, T>::value, T>::operator =;
        };

    public:
        Data() {}

        Data(const Data& data)
            :Info(data.jsonValue())
        {}

        Data(const Json::Value& jsonValue)
            :Info(jsonValue)
        {}

        bool init(const std::string& str)
        {
            return Json::Reader().parse(str, jsonValue_, false);
        }

        Info key(const std::string& key)
        {
            return Info(this, key);
        }

        Data& operator = (const Data& data)
        {
            if (this != &data)
            {
                jsonValue() = data.jsonValue();
            }

            return *this;
        }

        Data& operator = (const Json::Value& json)
        {
            jsonValue() = json;

            return *this;
        }

        operator Json::Value() const
        {
            return jsonValue();
        }

        std::string toString() const
        {
            return Json::FastWriter().write(jsonValue());
        }

        std::string toStyledString() const
        {
            return Json::StyledWriter().write(jsonValue());
        }
    };
}


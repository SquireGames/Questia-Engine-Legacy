#include "QuestiaEng/StateManager/States/State_OptionsMenu/Option.h"

template <class T>
Option<T>::Option()
{

}

template <class T>
void Option<T>::setType(OptionType _optionType)
{
    optionType = _optionType;
}

template <class T>
void Option<T>::init(T value)
{
    oldOption = value;
    newOption = value;

    switch (optionType)
    {
    case OptionType::choice:
    {
        for (auto it = choiceList.begin(); it != choiceList.end(); it++)
        {
            if(it->second == value)
            {
                choiceIterator = it;
                visibleOption = it->first;
                break;
            }
        }
    }
    break;
    case OptionType::functional:
        break;
    case OptionType::input:
        visibleOption = ctr::getInputName(value);
        break;
    }
}
template <>
void Option<std::string>::init(std::string value)
{
    oldOption = value;
    newOption = value;

    switch (optionType)
    {
    case OptionType::choice:
    {
        for (auto it = choiceList.begin(); it != choiceList.end(); it++)
        {
            if(it->second == value)
            {
                choiceIterator = it;
                visibleOption = it->first;
                break;
            }
        }
    }
    break;
    case OptionType::functional:
        break;
    case OptionType::input:
        visibleOption = value;
        break;
    }
}

template <class T>
bool Option<T>::isChanged()
{
    return (oldOption != newOption);
}

template <class T>
void Option<T>::setOptionName (std::string _optionName)
{
    optionName = _optionName;
}

template <class T>
std::string Option<T>::getOptionName ()
{
    return optionName;
}

template <class T>
T Option<T>::getValue()
{
    return newOption;
}

template <class T>
std::string Option<T>::getValueString()
{
    return utl::asString(newOption);
}

template <>
std::string Option<ctr::Input>::getValueString()
{
    return utl::asString(static_cast<int>(newOption));
}

template <class T>
void Option<T>::addChoice (std::pair <std::string, T> option)
{
    choiceList.push_back(option);
}

template <class T>
void Option<T>::setChoices (std::list <std::pair <std::string, T> > _choiceList)
{
    choiceList = _choiceList;
}

template <class T>
void Option<T>::setList(std::string _listName)
{
    listName = _listName;
}

template <class T>
std::string Option<T>::getList()
{
    return listName;
}

template <class T>
std::string* Option<T>::getDisplayString()
{
    return &visibleOption;
}

template <class T>
OptionType Option<T>::getType()
{
    return optionType;
}

template <class T>
void Option<T>::iterateForward()
{
    if(choiceIterator != --choiceList.end())
    {
        choiceIterator++;
        visibleOption = choiceIterator->first;

        newOption = choiceIterator->second;
    }
}

template <class T>
void Option<T>::iterateBackward()
{
    if(choiceIterator != choiceList.begin())
    {
        choiceIterator--;
        visibleOption = choiceIterator->first;
        newOption = choiceIterator->second;
    }
}

template <class T>
bool Option<T>::isEnd()
{
    if(choiceIterator == --choiceList.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <class T>
bool Option<T>::isBegin()
{
    if(choiceIterator == choiceList.begin())
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <class T>
void Option<T>::callFunction()
{
    //not implemented
}

template <class T>
void Option<T>::setInput(ctr::Input input)
{

}

template <>
void Option<ctr::Input>::setInput(ctr::Input input)
{
    visibleOption = ctr::getInputName(input);
    newOption = input;
}

template class Option<bool>;
template class Option<char>;
template class Option<int>;
template class Option<float>;
template class Option<std::string>;
template class Option<ctr::Input>;

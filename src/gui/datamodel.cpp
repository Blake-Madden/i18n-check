/////////////////////////////////////////////////////////////////////////////
// Name:        datamodel.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
/////////////////////////////////////////////////////////////////////////////

#include "datamodel.h"

//------------------------------------------------------
I18NResultsTreeModel::I18NResultsTreeModel()
    : m_root(new I18NResultsTreeModelNode(nullptr, wxString{}))
    {
    }

//------------------------------------------------------
void I18NResultsTreeModel::AddRow(wxString fileName, wxString warningId, wxString issue,
                                  wxString explanation, const int line, const int column)
    {
    wxASSERT(m_root);

    const auto unquote = [](wxString& strVal)
    {
        strVal.Trim(false);
        if (!strVal.empty() && strVal[0] == L'\"')
            {
            strVal.erase(0, 1);
            }
        if (!strVal.empty() && strVal[strVal.length() - 1] == L'\"')
            {
            strVal.erase(strVal.length() - 1);
            }

        return strVal;
    };

    for (auto& currentNode : m_root->GetChildren())
        {
        if (*currentNode == fileName)
            {
            I18NResultsTreeModelNode* childNode = new I18NResultsTreeModelNode(
                currentNode.get(), unquote(fileName), unquote(warningId), unquote(issue),
                unquote(explanation), line, column);
            currentNode->Append(childNode);

            return;
            }
        }

    auto newFile = new I18NResultsTreeModelNode(m_root, fileName);
    m_root->Append(newFile);

    I18NResultsTreeModelNode* childNode =
        new I18NResultsTreeModelNode(newFile, unquote(fileName), unquote(warningId), unquote(issue),
                                     unquote(explanation), line, column);
    newFile->Append(childNode);
    }

//------------------------------------------------------
void I18NResultsTreeModel::Delete(const wxDataViewItem& item)
    {
    I18NResultsTreeModelNode* node = reinterpret_cast<I18NResultsTreeModelNode*>(item.GetID());
    if (node == nullptr)
        {
        return;
        }

    wxDataViewItem parent(node->GetParent());
    if (!parent.IsOk())
        {
        wxASSERT(node == m_root);

        // don't make the control completely empty:
        wxLogError(L"Cannot remove the root item!");
        return;
        }

    // first remove the node from the parent's array of children;
    auto& siblings = node->GetParent()->GetChildren();
    for (auto it = siblings.begin(); it != siblings.end(); ++it)
        {
        if (it->get() == node)
            {
            siblings.erase(it);
            break;
            }
        }

    // notify control
    ItemDeleted(parent, item);
    }

//------------------------------------------------------
void I18NResultsTreeModel::Clear()
    {
    m_root->GetChildren().clear();

    Cleared();
    }

//------------------------------------------------------
int I18NResultsTreeModel::Compare(const wxDataViewItem& item1, const wxDataViewItem& item2,
                                  unsigned int column, bool ascending) const
    {
    wxASSERT(item1.IsOk() && item2.IsOk());
    // should never happen

    if (IsContainer(item1) && IsContainer(item2))
        {
        wxVariant value1, value2;
        GetValue(value1, item1, 0);
        GetValue(value2, item2, 0);

        int res = value1.GetString().Cmp(value2.GetString());
        if (res != 0)
            {
            return res;
            }

        // items must be different
        wxUIntPtr litem1 = reinterpret_cast<wxUIntPtr>(item1.GetID());
        wxUIntPtr litem2 = reinterpret_cast<wxUIntPtr>(item2.GetID());

        return litem1 - litem2;
        }

    return wxDataViewModel::Compare(item1, item2, column, ascending);
    }

//------------------------------------------------------
void I18NResultsTreeModel::GetValue(wxVariant& variant, const wxDataViewItem& item,
                                    unsigned int col) const
    {
    wxASSERT(item.IsOk());

    const I18NResultsTreeModelNode* node =
        reinterpret_cast<I18NResultsTreeModelNode*>(item.GetID());
    switch (col)
        {
    case 0:
        {
        wxBitmapBundle bmps;
        if (wxFileName::Exists(node->m_warningId))
            {
            bmps = wxArtProvider::GetBitmapBundle(wxART_NEW, wxART_BUTTON);
            }
        else if (node->m_warningId == L"[printfMismatch]" ||
                 node->m_warningId == L"[suspectL10NString]" ||
                 node->m_warningId == L"[suspectL10NUsage]" ||
                 node->m_warningId == L"[urlInL10NString]" ||
                 node->m_warningId == L"[notL10NAvailable]")
            {
            bmps = wxArtProvider::GetBitmapBundle(L"ID_TRANSLATIONS", wxART_OTHER);
            }
        else if (node->m_warningId == L"[fontIssue]" || node->m_warningId == L"[deprecatedMacro]" ||
                 node->m_warningId == L"[printfSingleNumber]" ||
                 node->m_warningId == L"[dupValAssignedToIds]" ||
                 node->m_warningId == L"[numberAssignedToId]" ||
                 node->m_warningId == L"[malformedString]")
            {
            bmps = wxArtProvider::GetBitmapBundle(L"ID_CODE", wxART_OTHER);
            }
        else if (node->m_warningId == L"[nonUTF8File]" ||
                 node->m_warningId == L"[UTF8FileWithBOM]" ||
                 node->m_warningId == L"[unencodedExtASCII]" ||
                 node->m_warningId == L"[trailingSpaces]" || node->m_warningId == L"[tabs]" ||
                 node->m_warningId == L"[wideLine]" ||
                 node->m_warningId == L"[commentMissingSpace]")
            {
            bmps = wxArtProvider::GetBitmapBundle(L"ID_FORMAT", wxART_OTHER);
            }
        else if (node->m_warningId == L"[debugParserInfo]")
            {
            bmps = wxArtProvider::GetBitmapBundle(L"ID_DEBUG", wxART_OTHER);
            }
        variant = static_cast<wxVariant>(wxDataViewIconText(
            wxFileName::Exists(node->m_warningId) ? wxFileName{ node->m_warningId }.GetFullName() :
                                                    node->m_warningId,
            bmps));
        }
        break;
    case 1:
        variant = node->m_issue;
        break;
    case 2:
        if (node->m_line != -1)
            {
            variant = static_cast<long>(node->m_line);
            }
        break;
    case 3:
        if (node->m_column != -1)
            {
            variant = static_cast<long>(node->m_column);
            }
        break;
    case 4:
        variant = node->m_explaination;
        break;
    default:
        wxLogError(L"I18NResultsTreeModel::GetValue(): wrong column %d", col);
        }
    }

//------------------------------------------------------
bool I18NResultsTreeModel::SetValue(const wxVariant& variant, const wxDataViewItem& item,
                                    unsigned int col)
    {
    wxASSERT(item.IsOk());

    I18NResultsTreeModelNode* node = reinterpret_cast<I18NResultsTreeModelNode*>(item.GetID());
    switch (col)
        {
    case 0:
        node->m_warningId = variant.GetString();
        return true;
    case 1:
        node->m_issue = variant.GetString();
        return true;
    case 2:
        node->m_line = variant.GetLong();
        return true;
    case 3:
        node->m_column = variant.GetLong();
        return true;
    case 4:
        node->m_explaination = variant.GetString();
        return true;
    default:
        wxLogError(L"I18NResultsTreeModel::SetValue(): wrong column");
        }
    return false;
    }

//------------------------------------------------------
wxDataViewItem I18NResultsTreeModel::GetParent(const wxDataViewItem& item) const
    {
    // the invisible root node has no parent
    if (!item.IsOk())
        {
        return wxDataViewItem(nullptr);
        }

    I18NResultsTreeModelNode* node = reinterpret_cast<I18NResultsTreeModelNode*>(item.GetID());

    // "I18NResults" also has no parent
    if (node == m_root)
        {
        return wxDataViewItem(nullptr);
        }

    return wxDataViewItem(reinterpret_cast<void*>(node->GetParent()));
    }

//------------------------------------------------------
bool I18NResultsTreeModel::IsContainer(const wxDataViewItem& item) const
    {
    // the invisible root node can have children
    // (in our model always "I18NResults")
    if (!item.IsOk())
        {
        return true;
        }

    I18NResultsTreeModelNode* node = reinterpret_cast<I18NResultsTreeModelNode*>(item.GetID());
    return node->IsContainer();
    }

//------------------------------------------------------
unsigned int I18NResultsTreeModel::GetChildren(const wxDataViewItem& parent,
                                               wxDataViewItemArray& array) const
    {
    I18NResultsTreeModelNode* node = reinterpret_cast<I18NResultsTreeModelNode*>(parent.GetID());
    if (!node)
        {
        return GetChildren(wxDataViewItem(m_root), array);
        }

    if (node->GetChildCount() == 0)
        {
        return 0;
        }

    for (const auto& child : node->GetChildren())
        {
        array.Add(wxDataViewItem(child.get()));
        }

    return array.size();
    }

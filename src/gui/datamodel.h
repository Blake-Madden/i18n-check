/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2024
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __I18N_DATAMODEL_H__
#define __I18N_DATAMODEL_H__

#include <memory>
#include <unordered_map>
#include <vector>
#include <wx/artprov.h>
#include <wx/dataview.h>
#include <wx/wx.h>

// ----------------------------------------------------------------------------
// I18NResultsTreeModelNode: a node inside I18NResultsTreeModel
// ----------------------------------------------------------------------------

class I18NResultsTreeModelNode;
using I18NResultsTreeModelNodePtr = std::unique_ptr<I18NResultsTreeModelNode>;
using I18NResultsTreeModelNodePtrArray = std::vector<I18NResultsTreeModelNodePtr>;

class I18NResultsTreeModelNode
    {
  public:
    I18NResultsTreeModelNode(I18NResultsTreeModelNode* parent, const wxString& warningId,
                             const wxString& issue, const wxString& explanation, const int line,
                             const int column)
        : m_warningId(warningId), m_issue(issue), m_explaination(explanation), m_line(line),
          m_column(column), m_container(false), m_parent(parent)
        {
        }

    I18NResultsTreeModelNode(I18NResultsTreeModelNode* parent, const wxString& warningId)
        : m_warningId(warningId), m_container(true), m_parent(parent)
        {
        }

    ~I18NResultsTreeModelNode() = default;

    bool IsContainer() const { return m_container; }

    I18NResultsTreeModelNode* GetParent() { return m_parent; }

    I18NResultsTreeModelNodePtrArray& GetChildren() { return m_children; }

    I18NResultsTreeModelNode* GetNthChild(unsigned int n) { return m_children.at(n).get(); }

    void Insert(I18NResultsTreeModelNode* child, unsigned int n)
        {
        m_children.insert(m_children.begin() + n, I18NResultsTreeModelNodePtr(child));
        }

    void Append(I18NResultsTreeModelNode* child)
        {
        m_children.push_back(I18NResultsTreeModelNodePtr(child));
        }

    size_t GetChildCount() const { return m_children.size(); }

  public:
    wxString m_warningId;
    wxString m_issue;
    wxString m_explaination;
    int m_line{ -1 };
    int m_column{ -1 };

    bool m_container{ true };

  private:
    I18NResultsTreeModelNode* m_parent;
    I18NResultsTreeModelNodePtrArray m_children;
    };

// Results renderer.
class I18NResultsTreeModel : public wxDataViewModel
    {
  public:
    I18NResultsTreeModel();

    ~I18NResultsTreeModel() { delete m_root; }

    // helper methods to change the model

    void Delete(const wxDataViewItem& item);
    void Clear();

    void AddRow(const wxString& fileName, const wxString& warningId, const wxString& issue,
                const wxString& explanation, const int line, const int column);

    // override sorting to always sort branches ascendingly
    int Compare(const wxDataViewItem& item1, const wxDataViewItem& item2, unsigned int column,
                bool ascending) const final;

    // implementation of base class virtuals to define model
    void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const final;
    bool SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col) final;

    wxDataViewItem GetParent(const wxDataViewItem& item) const final;
    bool IsContainer(const wxDataViewItem& item) const final;
    unsigned int GetChildren(const wxDataViewItem& parent, wxDataViewItemArray& array) const final;

  private:
    I18NResultsTreeModelNode* m_root{ nullptr };
    };

    /** @}*/

#endif //__I18N_DATAMODEL_H__

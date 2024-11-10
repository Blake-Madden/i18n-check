/********************************************************************************
 * Copyright (c) 2024 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/
 
/** @addtogroup Internationalization
    @brief i18n classes.
@{*/

#ifndef __I18N_DATAMODEL_H__
#define __I18N_DATAMODEL_H__

#include <memory>
#include <unordered_map>
#include <vector>
#include <wx/artprov.h>
#include <wx/dataview.h>
#include <wx/filename.h>
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
    I18NResultsTreeModelNode(I18NResultsTreeModelNode* parent, wxString fileName,
                             wxString warningId, wxString issue, wxString explanation,
                             const int line, const int column)
        : m_fileName(std::move(fileName)), m_warningId(std::move(warningId)),
          m_issue(std::move(issue)), m_explaination(std::move(explanation)), m_line(line),
          m_column(column), m_container(false), m_parent(parent)
        {
        }

    // Root node for file.
    // Filename is copied into warning ID so that it gets rendered at the root level.
    I18NResultsTreeModelNode(I18NResultsTreeModelNode* parent, const wxString& fileName)
        : m_fileName(fileName), m_warningId(fileName), m_container(true), m_parent(parent)
        {
        }

    ~I18NResultsTreeModelNode() = default;

    bool IsContainer() const { return m_container; }

    I18NResultsTreeModelNode* GetParent() { return m_parent; }

    I18NResultsTreeModelNodePtrArray& GetChildren() { return m_children; }

    void Insert(I18NResultsTreeModelNode* child, unsigned int n)
        {
        m_children.insert(m_children.begin() + n, I18NResultsTreeModelNodePtr(child));
        }

    void Append(I18NResultsTreeModelNode* child)
        {
        m_children.push_back(I18NResultsTreeModelNodePtr(child));
        }

    size_t GetChildCount() const { return m_children.size(); }

    [[nodiscard]]
    bool
    operator==(const wxString& fileName) const
        {
        return m_fileName.CmpNoCase(fileName) == 0;
        }

  public:
    wxString m_fileName;
    wxString m_warningId;
    wxString m_issue;
    wxString m_explaination;
    int m_line{ -1 };
    int m_column{ -1 };

    bool m_container{ true };

  private:
    I18NResultsTreeModelNode* m_parent{ nullptr };
    I18NResultsTreeModelNodePtrArray m_children;
    };

// Results renderer.
class I18NResultsTreeModel : public wxDataViewModel
    {
  public:
    I18NResultsTreeModel();

    ~I18NResultsTreeModel() { delete m_root; }

    // helper methods to change the model

    wxDataViewItem GetRoot() { return wxDataViewItem(reinterpret_cast<void*>(m_root)); }

    void Delete(const wxDataViewItem& item);
    void DeleteWarning(const wxString& warningId);
    void Clear();

    void AddRow(wxString fileName, wxString warningId, wxString issue, wxString explanation,
                const int line, const int column);

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

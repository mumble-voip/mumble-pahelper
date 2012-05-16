/* Copyright (C) 2010, Stefan Hacker <dd0t@users.sourceforge.net>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MUMBLEPAHELPER_H
#define MUMBLEPAHELPER_H

#include <QMainWindow>
#include <QMessageBox>
#include "Plugins.h"
#include "ui_MumblePAHelper.h"


class MumblePAHelper : public QMainWindow, public Ui::MumblePAHelper
{
private:
    Q_OBJECT
    Q_DISABLE_COPY(MumblePAHelper)
    Plugins *plugins;
public:
    explicit MumblePAHelper(QWidget *parent = 0);
    ~MumblePAHelper();

public slots:
    void on_plugins_Fetched();
    void on_plugins_IdentityChanged(const QString);
    void on_plugins_ContextChanged(const QString);
    void on_plugins_LinkLost(const PluginInfo*);
    void on_plugins_Linked(const PluginInfo*);
    void on_plugins_PluginList(const QList<PluginInfo*>);

    void on_qpbConfig_clicked(bool);
    void on_qlwPlugins_currentItemChanged(QListWidgetItem*, QListWidgetItem*);

    void on_action_Unlink_triggered(bool);
    void on_action_Quit_triggered(bool);
    void on_action_Rescan_triggered(bool);
};

#endif // MUMBLEPAHELPER_H

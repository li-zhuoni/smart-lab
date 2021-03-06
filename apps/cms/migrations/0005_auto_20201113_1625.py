# Generated by Django 3.0.7 on 2020-11-13 08:25

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('cms', '0004_borrow_borrow_item_id'),
    ]

    operations = [
        migrations.CreateModel(
            name='Item',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('item_name', models.CharField(blank=True, max_length=200, null=True)),
                ('item_uid', models.CharField(max_length=200, null=True, unique=True)),
                ('item_num', models.IntegerField(blank=True, max_length=200, null=True)),
                ('item_shengcan_date', models.DateTimeField(auto_now_add=True, null=True)),
                ('item_daoqi_date', models.DateTimeField(auto_now_add=True, null=True)),
            ],
        ),
        migrations.AddField(
            model_name='labtoratory',
            name='lab_temperature_people',
            field=models.IntegerField(blank=True, max_length=100, null=True),
        ),
        migrations.AddField(
            model_name='member',
            name='lab',
            field=models.ForeignKey(null=True, on_delete=django.db.models.deletion.CASCADE, to='cms.Labtoratory'),
        ),
        migrations.AddField(
            model_name='member',
            name='member_temperature',
            field=models.FloatField(blank=True, max_length=50, null=True),
        ),
        migrations.CreateModel(
            name='reservation',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('reservation_time', models.CharField(max_length=100)),
                ('reservation_reason', models.CharField(max_length=200)),
                ('member', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='cms.Member')),
            ],
        ),
        migrations.CreateModel(
            name='repair',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('repair_time', models.DateTimeField(auto_now_add=True)),
                ('repair_reason', models.CharField(blank=True, max_length=200, null=True)),
                ('item', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='cms.Item')),
                ('member', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='cms.Member')),
            ],
        ),
    ]
